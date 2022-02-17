/*
 * mms_server.c 參考libiec61850/examples/server_example_basic_io/server_example_basic_io.c.
 *
 *  Created on: 2022年2月3日
 *      Author: ken
 *
 *  MMS Data
 *      |_ DI (DataSet: DIDRE, change與update時,發出report)
 *      |_ AI
 *          |_ 報價代碼(Group, DataSet: AIGRO, 每10秒鐘發出report)
 *          |_ 交易資源(Resource)
 *                   |_階層項目(DataSet: ITEM, change與update時,發出report)
 *                   |_量測項目(電力數據資料, DataSet: AIDRE, 每10秒鐘發出report)
 */

#include "iec61850_server.h"
#include "hal_thread.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "static_model.h"

#define  PI 3.1415926
#define  MMS_PORT 102

// import IEC 61850 device model created from SCL-File
extern   IedModel iedModel;

static   int running = 0;
static   IedServer iedServer = NULL;
static void connectionHandler (IedServer self, ClientConnection connection, bool connected, void* parameter) {
    if (connected)
        printf("Connection opened\n");
    else
        printf("Connection closed\n");
}

void sigint_handler(int signalId) {
    running = 0;
}

// 報價代碼聚合之所有交易資源加總之瞬時輸出/入總實功率
struct GroupRealPowerStruc {
	// 所有交易資源第N秒鐘加總之瞬時輸出/入總實功率,單位為0.01kW
	int totalWatt;
	// 所有交易資源第N秒鐘加總之瞬時輸出/入總實功率之時間點,UTC時間,最小單位為秒
	unsigned long timestamp;
};
typedef struct GroupRealPowerStruc GroupRealPower;

// AFC & dReg 電力數據資料欄位
struct PowerInfoStruc {
	// 瞬時R相電壓,單位為0.01kV
	unsigned int phvA;
	// 瞬時S相電壓,單位為0.01kV
	unsigned int phvB;
	// 瞬時T相電壓,單位為0.01kV
	unsigned int phvC;
	// 瞬時R相電流,單位為0.01A
	unsigned int currA;
	// 瞬時S相電流,單位為0.01A
	unsigned int currB;
	// 瞬時T相電流,單位為0.01A
	unsigned int currC;
	// 瞬時系統頻率,單位為0.01Hz
	unsigned int freq;
	// 瞬時輸出/入總實功率,單位為0.01kW
	unsigned int totW;
	// 瞬時總虛功率,單位為0.01kVar
	unsigned int totVAr;
	// 瞬時總功率因數,單位為 0.01
	unsigned int totPF;
	// 儲能系統瞬時剩餘電量 SOC,單位為0.01%
	unsigned int soc;
	// 儲能系統狀態, 0:無異常,1:異常
	bool essState;
	// 提供該秒資料之時間點(Unix Timestamp-H),UTC時間,時間最小單位為秒
	unsigned long timestamp;
};
typedef struct PowerInfoStruc ResourcePowerInfo;

// DI(DataSet: DIDRE) and AI(DataSet: AIGRO) of 報價代碼
struct GroupInputStruc {
	// 履行服務開始
	bool serviceStart;
	// 履行服務結束
	bool serviceStop;
	// 合格交易者代碼(統一編號)
	unsigned int providerId;
	// 報價代碼(最多5碼)
	unsigned int quotationId;
	// 輔助服務商品(AFC: 1)
	unsigned char serviceCode;
	// 所有交易資源1-10秒加總之瞬時輸出/入總實功率,
	GroupRealPower realPower[10];
	// 報價代碼聚合之所有交易資源加總瞬時累計發電量
	unsigned long supplyWatt;
	// 報價代碼聚合之所有交易資源加總瞬時累計用電量
	unsigned long demandWatt;
	// 執行率計算時間點
	uint64_t timestamp;
	// 執行率
	unsigned int executionRate;
};
typedef struct GroupInputStruc GroupInput;

// AI of 交易資源, including 階層項目(DataSet: ITEM) &量測項目(電力數據資料, DataSet: AIDRE)
struct ResourceInputStruc {
	// 合格交易者代碼(統一編號)
	unsigned int providerId;
	// 報價代碼(最多5碼)
	unsigned int quotationId;
	// 交易資源代碼(最多5碼)
	unsigned int resourceId;
	// 輔助服務商品(AFC: 1)
	unsigned char serviceCode;
	// 所有交易資源1-10秒電力數據資料
	ResourcePowerInfo powerInfo[10];
};
typedef struct ResourceInputStruc ResourceInput;

DataAttribute *groupRealPowerTsHighDaNames[10] = {
		IEDMODEL_G8477_GROGGIO01_AnIn1_mag_i,
		IEDMODEL_G8477_GROGGIO02_AnIn1_mag_i,
		IEDMODEL_G8477_GROGGIO03_AnIn1_mag_i,
		IEDMODEL_G8477_GROGGIO04_AnIn1_mag_i,
		IEDMODEL_G8477_GROGGIO05_AnIn1_mag_i,
		IEDMODEL_G8477_GROGGIO06_AnIn1_mag_i,
		IEDMODEL_G8477_GROGGIO07_AnIn1_mag_i,
		IEDMODEL_G8477_GROGGIO08_AnIn1_mag_i,
		IEDMODEL_G8477_GROGGIO09_AnIn1_mag_i,
		IEDMODEL_G8477_GROGGIO10_AnIn1_mag_i };

DataAttribute *groupRealPowerTsLowDaNames[10] = {
		IEDMODEL_G8477_GROGGIO01_AnIn2_mag_i,
		IEDMODEL_G8477_GROGGIO02_AnIn2_mag_i,
		IEDMODEL_G8477_GROGGIO03_AnIn2_mag_i,
		IEDMODEL_G8477_GROGGIO04_AnIn2_mag_i,
		IEDMODEL_G8477_GROGGIO05_AnIn2_mag_i,
		IEDMODEL_G8477_GROGGIO06_AnIn2_mag_i,
		IEDMODEL_G8477_GROGGIO07_AnIn2_mag_i,
		IEDMODEL_G8477_GROGGIO08_AnIn2_mag_i,
		IEDMODEL_G8477_GROGGIO09_AnIn2_mag_i,
		IEDMODEL_G8477_GROGGIO10_AnIn2_mag_i };

DataAttribute *groupRealPowerDaNames[10] = {
		IEDMODEL_G8477_GROMMXU01_TotW_mag_i,
		IEDMODEL_G8477_GROMMXU02_TotW_mag_i,
		IEDMODEL_G8477_GROMMXU03_TotW_mag_i,
		IEDMODEL_G8477_GROMMXU04_TotW_mag_i,
		IEDMODEL_G8477_GROMMXU05_TotW_mag_i,
		IEDMODEL_G8477_GROMMXU06_TotW_mag_i,
		IEDMODEL_G8477_GROMMXU07_TotW_mag_i,
		IEDMODEL_G8477_GROMMXU08_TotW_mag_i,
		IEDMODEL_G8477_GROMMXU09_TotW_mag_i,
		IEDMODEL_G8477_GROMMXU10_TotW_mag_i };

DataAttribute *resourcePowerInfoPhvADaNames[10] = {
		IEDMODEL_R8623_DREMMXU01_PhV_phsA_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU02_PhV_phsA_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU03_PhV_phsA_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU04_PhV_phsA_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU05_PhV_phsA_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU06_PhV_phsA_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU07_PhV_phsA_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU08_PhV_phsA_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU09_PhV_phsA_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU10_PhV_phsA_cVal_mag_i };

DataAttribute *resourcePowerInfoPhvBDaNames[10] = {
		IEDMODEL_R8623_DREMMXU01_PhV_phsB_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU02_PhV_phsB_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU03_PhV_phsB_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU04_PhV_phsB_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU05_PhV_phsB_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU06_PhV_phsB_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU07_PhV_phsB_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU08_PhV_phsB_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU09_PhV_phsB_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU10_PhV_phsB_cVal_mag_i };

DataAttribute *resourcePowerInfoPhvCDaNames[10] = {
		IEDMODEL_R8623_DREMMXU01_PhV_phsC_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU02_PhV_phsC_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU03_PhV_phsC_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU04_PhV_phsC_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU05_PhV_phsC_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU06_PhV_phsC_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU07_PhV_phsC_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU08_PhV_phsC_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU09_PhV_phsC_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU10_PhV_phsC_cVal_mag_i };

DataAttribute *resourcePowerInfoCurrADaNames[10] = {
		IEDMODEL_R8623_DREMMXU01_A_phsA_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU02_A_phsA_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU03_A_phsA_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU04_A_phsA_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU05_A_phsA_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU06_A_phsA_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU07_A_phsA_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU08_A_phsA_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU09_A_phsA_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU10_A_phsA_cVal_mag_i };

DataAttribute *resourcePowerInfoCurrBDaNames[10] = {
		IEDMODEL_R8623_DREMMXU01_A_phsB_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU02_A_phsB_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU03_A_phsB_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU04_A_phsB_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU05_A_phsB_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU06_A_phsB_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU07_A_phsB_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU08_A_phsB_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU09_A_phsB_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU10_A_phsB_cVal_mag_i };

DataAttribute *resourcePowerInfoCurrCDaNames[10] = {
		IEDMODEL_R8623_DREMMXU01_A_phsC_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU02_A_phsC_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU03_A_phsC_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU04_A_phsC_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU05_A_phsC_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU06_A_phsC_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU07_A_phsC_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU08_A_phsC_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU09_A_phsC_cVal_mag_i,
		IEDMODEL_R8623_DREMMXU10_A_phsC_cVal_mag_i };

DataAttribute *resourcePowerInfoHzDaNames[10] = {
		IEDMODEL_R8623_DREMMXU01_Hz_mag_i, IEDMODEL_R8623_DREMMXU02_Hz_mag_i,
		IEDMODEL_R8623_DREMMXU03_Hz_mag_i, IEDMODEL_R8623_DREMMXU04_Hz_mag_i,
		IEDMODEL_R8623_DREMMXU05_Hz_mag_i, IEDMODEL_R8623_DREMMXU06_Hz_mag_i,
		IEDMODEL_R8623_DREMMXU07_Hz_mag_i, IEDMODEL_R8623_DREMMXU08_Hz_mag_i,
		IEDMODEL_R8623_DREMMXU09_Hz_mag_i, IEDMODEL_R8623_DREMMXU10_Hz_mag_i };

DataAttribute *resourcePowerInfoTotWDaNames[10] = {
		IEDMODEL_R8623_DREMMXU01_TotW_mag_i,
		IEDMODEL_R8623_DREMMXU02_TotW_mag_i,
		IEDMODEL_R8623_DREMMXU03_TotW_mag_i,
		IEDMODEL_R8623_DREMMXU04_TotW_mag_i,
		IEDMODEL_R8623_DREMMXU05_TotW_mag_i,
		IEDMODEL_R8623_DREMMXU06_TotW_mag_i,
		IEDMODEL_R8623_DREMMXU07_TotW_mag_i,
		IEDMODEL_R8623_DREMMXU08_TotW_mag_i,
		IEDMODEL_R8623_DREMMXU09_TotW_mag_i,
		IEDMODEL_R8623_DREMMXU10_TotW_mag_i };

DataAttribute *resourcePowerInfoTotVArDaNames[10] = {
		IEDMODEL_R8623_DREMMXU01_TotVAr_mag_i,
		IEDMODEL_R8623_DREMMXU02_TotVAr_mag_i,
		IEDMODEL_R8623_DREMMXU03_TotVAr_mag_i,
		IEDMODEL_R8623_DREMMXU04_TotVAr_mag_i,
		IEDMODEL_R8623_DREMMXU05_TotVAr_mag_i,
		IEDMODEL_R8623_DREMMXU06_TotVAr_mag_i,
		IEDMODEL_R8623_DREMMXU07_TotVAr_mag_i,
		IEDMODEL_R8623_DREMMXU08_TotVAr_mag_i,
		IEDMODEL_R8623_DREMMXU09_TotVAr_mag_i,
		IEDMODEL_R8623_DREMMXU10_TotVAr_mag_i };

DataAttribute *resourcePowerInfoTotPFDaNames[10] = {
		IEDMODEL_R8623_DREMMXU01_TotPF_mag_i,
		IEDMODEL_R8623_DREMMXU02_TotPF_mag_i,
		IEDMODEL_R8623_DREMMXU03_TotPF_mag_i,
		IEDMODEL_R8623_DREMMXU04_TotPF_mag_i,
		IEDMODEL_R8623_DREMMXU05_TotPF_mag_i,
		IEDMODEL_R8623_DREMMXU06_TotPF_mag_i,
		IEDMODEL_R8623_DREMMXU07_TotPF_mag_i,
		IEDMODEL_R8623_DREMMXU08_TotPF_mag_i,
		IEDMODEL_R8623_DREMMXU09_TotPF_mag_i,
		IEDMODEL_R8623_DREMMXU10_TotPF_mag_i };

DataAttribute *resourcePowerInfoSocDaNames[10] = {
		IEDMODEL_R8623_DREZBAT01_InBatV_mag_i,
		IEDMODEL_R8623_DREZBAT02_InBatV_mag_i,
		IEDMODEL_R8623_DREZBAT03_InBatV_mag_i,
		IEDMODEL_R8623_DREZBAT04_InBatV_mag_i,
		IEDMODEL_R8623_DREZBAT05_InBatV_mag_i,
		IEDMODEL_R8623_DREZBAT06_InBatV_mag_i,
		IEDMODEL_R8623_DREZBAT07_InBatV_mag_i,
		IEDMODEL_R8623_DREZBAT08_InBatV_mag_i,
		IEDMODEL_R8623_DREZBAT09_InBatV_mag_i,
		IEDMODEL_R8623_DREZBAT10_InBatV_mag_i };

DataAttribute *resourcePowerInfoBatStatDaNames[10] = {
		IEDMODEL_R8623_DREZBAT01_BatSt_stVal,
		IEDMODEL_R8623_DREZBAT02_BatSt_stVal,
		IEDMODEL_R8623_DREZBAT03_BatSt_stVal,
		IEDMODEL_R8623_DREZBAT04_BatSt_stVal,
		IEDMODEL_R8623_DREZBAT05_BatSt_stVal,
		IEDMODEL_R8623_DREZBAT06_BatSt_stVal,
		IEDMODEL_R8623_DREZBAT07_BatSt_stVal,
		IEDMODEL_R8623_DREZBAT08_BatSt_stVal,
		IEDMODEL_R8623_DREZBAT09_BatSt_stVal,
		IEDMODEL_R8623_DREZBAT10_BatSt_stVal };

DataAttribute *resourcePowerInfoTsHighDaNames[10] = {
		IEDMODEL_R8623_DREGGIO01_AnIn1_mag_i,
		IEDMODEL_R8623_DREGGIO02_AnIn1_mag_i,
		IEDMODEL_R8623_DREGGIO03_AnIn1_mag_i,
		IEDMODEL_R8623_DREGGIO04_AnIn1_mag_i,
		IEDMODEL_R8623_DREGGIO05_AnIn1_mag_i,
		IEDMODEL_R8623_DREGGIO06_AnIn1_mag_i,
		IEDMODEL_R8623_DREGGIO07_AnIn1_mag_i,
		IEDMODEL_R8623_DREGGIO08_AnIn1_mag_i,
		IEDMODEL_R8623_DREGGIO09_AnIn1_mag_i,
		IEDMODEL_R8623_DREGGIO10_AnIn1_mag_i };

DataAttribute *resourcePowerInfoTsLowDaNames[10] = {
		IEDMODEL_R8623_DREGGIO01_AnIn2_mag_i,
		IEDMODEL_R8623_DREGGIO02_AnIn2_mag_i,
		IEDMODEL_R8623_DREGGIO03_AnIn2_mag_i,
		IEDMODEL_R8623_DREGGIO04_AnIn2_mag_i,
		IEDMODEL_R8623_DREGGIO05_AnIn2_mag_i,
		IEDMODEL_R8623_DREGGIO06_AnIn2_mag_i,
		IEDMODEL_R8623_DREGGIO07_AnIn2_mag_i,
		IEDMODEL_R8623_DREGGIO08_AnIn2_mag_i,
		IEDMODEL_R8623_DREGGIO09_AnIn2_mag_i,
		IEDMODEL_R8623_DREGGIO10_AnIn2_mag_i };


// function prototypes
void simulate(GroupInput *groupInput, ResourceInput *resourceInput);
void printOut(GroupInput *groupInput, ResourceInput *resourceInput);
void updateIedModel(IedServer iedServer, GroupInput *groupInput, ResourceInput *resourceInput);

/**
 * Main function.
 */
int main(int argc, char** argv) {
	printf("Using libIEC61850 version %s\n", LibIEC61850_getVersionString());

	/* Create new server configuration object */
	IedServerConfig config = IedServerConfig_create();

	/* Set buffer size for buffered report control blocks to 200000 bytes */
	IedServerConfig_setReportBufferSize(config, 200000);

	/* Set stack compliance to a specific edition of the standard (WARNING: data model has also to be checked for compliance) */
	IedServerConfig_setEdition(config, IEC_61850_EDITION_2);

	/* Set the base path for the MMS file services */
	IedServerConfig_setFileServiceBasePath(config, "./vmd-filestore/");

	/* disable MMS file service */
	IedServerConfig_enableFileService(config, false);

	/* enable dynamic data set service */
	IedServerConfig_enableDynamicDataSetService(config, true);

	/* disable log service */
	IedServerConfig_enableLogService(config, false);

	/* set maximum number of clients */
	IedServerConfig_setMaxMmsConnections(config, 2);

	/* Create a new IEC 61850 server instance */
	iedServer = IedServer_createWithConfig(&iedModel, NULL, config);

	/* configuration object is no longer required */
	IedServerConfig_destroy(config);

	/* set the identity values for MMS identify service */
	IedServer_setServerIdentity(iedServer, "Hwacom", "mms servcer", "1.5");

	IedServer_setConnectionIndicationHandler(iedServer, (IedConnectionIndicationHandler) connectionHandler, NULL);

	IedServer_start(iedServer, MMS_PORT);

	if (!IedServer_isRunning(iedServer)) {
		printf("Starting server failed (maybe need root permissions or another server is already using the port)! Exit.\n");
		IedServer_destroy(iedServer);
		exit(-1);
	}

	running = 1;

	signal(SIGINT, sigint_handler);

	while (running) {
		// 輔助服務傳輸格式說明中，對時間精度的要求只到秒
		//uint64_t timestamp = Hal_getTimeInMs();
		uint32_t timestamp = time(NULL);
		Timestamp iecTimestamp;
		Timestamp_clearFlags(&iecTimestamp);
		//Timestamp_setTimeInMilliseconds(&iecTimestamp, timestamp);
		Timestamp_setTimeInSeconds(&iecTimestamp, timestamp);
		Timestamp_setLeapSecondKnown(&iecTimestamp, true);

		GroupInput *groupInput = malloc(sizeof(GroupInput));
		ResourceInput *resourceInput = malloc(sizeof(ResourceInput));
		simulate(groupInput, resourceInput);
		updateIedModel(iedServer, groupInput, resourceInput);
		printOut(groupInput, resourceInput);
		free(groupInput);
		free(resourceInput);
		Thread_sleep(1000);
	}

	/* stop MMS server - close TCP server socket and all client sockets */
	IedServer_stop(iedServer);

	/* Cleanup - free all resources */
	IedServer_destroy(iedServer);

	return 0;
}

void simulate(GroupInput *groupInput, ResourceInput *resourceInput) {
	groupInput->serviceStart = true;
	groupInput->serviceStop = false;
	groupInput->providerId = 84778623;
	groupInput->quotationId = 6163;
	groupInput->serviceCode = 0;
	groupInput->supplyWatt = 12000;
	groupInput->demandWatt = 10000;
	// 輔助服務傳輸格式說明中，對時間精度的要求只到秒
	// groupInput->timestamp = Hal_getTimeInMs();
	groupInput->timestamp = time(NULL);
  // 取報價代碼內第1個交易資源(Resource) ID之第5秒頻率及所有交易資源之第6秒加總實功率作為執行率計算依據
  // 根據反應曲線撰寫執行率計算公式
	groupInput->executionRate = 8888;
	for(int i=0; i<10; i++) {
		GroupRealPower *groupRealPower = malloc(sizeof(GroupRealPower));
		// 假設所有交易資源總輸出功率以360秒（6分鐘）週期在60KW至100KW之間變動
		groupRealPower->totalWatt = 8000 + 2000 * sin(PI * ((groupInput->timestamp - (10 - i)) % 360) / 180);
		groupRealPower->timestamp = groupInput->timestamp - (10 - i);
		groupInput->realPower[i] = *groupRealPower;
	}

	resourceInput->providerId = groupInput->providerId;
	resourceInput->quotationId = groupInput->quotationId;
	resourceInput->resourceId = 8623;
	resourceInput->serviceCode = groupInput->serviceCode;
	for(int i=0; i<10; i++) {
		ResourcePowerInfo *resourcePowerInfo = malloc(sizeof(ResourcePowerInfo));
		// 假設電壓以360秒（6分鐘）週期在23KV至22.6KV之間變動
		resourcePowerInfo->phvA = 2280 + 20 * sin(PI * ((groupInput->timestamp - (10 - i)) % 360) / 180);
		resourcePowerInfo->phvB = 2280 + 20 * sin(PI * ((groupInput->timestamp - (11 - i)) % 360) / 180);
		resourcePowerInfo->phvC = 2280 + 20 * sin(PI * ((groupInput->timestamp - (12 - i)) % 360) / 180);
		// 假設電流以360秒（6分鐘）週期在440A至360A之間變動
		resourcePowerInfo->currA = 40000 + 4000 * sin(PI * ((groupInput->timestamp - (10 - i)) % 360) / 180);
		resourcePowerInfo->currB = 40000 + 4000 * sin(PI * ((groupInput->timestamp - (11 - i)) % 360) / 180);
		resourcePowerInfo->currC = 40000 + 4000 * sin(PI * ((groupInput->timestamp - (12 - i)) % 360) / 180);
        // 假設頻率以360秒（6分鐘）週期在60.25至59.75Hz之間變動
		resourcePowerInfo->freq = 6000 + 25 * sin(PI * ((groupInput->timestamp - (10 - i)) % 360) / 180);
		// 只有一個交易資源，故等於報價代碼的TotW
		resourcePowerInfo->totW = groupInput->realPower[i].totalWatt;
		// 假設總虛功率以360秒（6分鐘）週期在20至60KVar之間變動
		resourcePowerInfo->totVAr = 4000 + 2000 * sin(PI * ((groupInput->timestamp - (10 - i)) % 360) / 180);
		// 假設總功率因數以360秒（6分鐘）週期在75至95%之間變動
		resourcePowerInfo->totPF = 8500 + 1000 * sin(PI * ((groupInput->timestamp - (10 - i)) % 360) / 180);
		// 假設剩餘電量SOC以360秒（6分鐘）週期在40至80%之間變動
		resourcePowerInfo->soc = 6000 + 2000 * sin(PI * ((groupInput->timestamp - (10 - i)) % 360) / 180);
		resourcePowerInfo->essState = 0;
		resourcePowerInfo->timestamp = groupInput->realPower[i].timestamp;
		resourceInput->powerInfo[i] = *resourcePowerInfo;
	}
}

void printOut(GroupInput *groupInput, ResourceInput *resourceInput) {
	printf("%s", "==================================================");
	// DI
	printf("%21s: %d\n", "service start", groupInput->serviceStart);
	printf("%21s: %d\n", "service stop", groupInput->serviceStop);
	// AI Group
	printf("%21s: %05d\n", "provider id", groupInput->providerId);
	printf("%21s: %05d\n", "quotation id", groupInput->quotationId);
	printf("%21s: %5d\n", "service code", groupInput->serviceCode);
	printf("%21s: %5ld\n", "service code", groupInput->supplyWatt);
	printf("%21s: %5ld\n", "service code", groupInput->demandWatt);
	for(int i=0; i<10; i++) {
		printf("real power at sec. %02d: %5d\n", i + 1, groupInput->realPower[i].totalWatt);
		printf("%21s: %ld\n", "timestamp", groupInput->realPower[i].timestamp);
	}
	printf("%21s: %5d\n", "execution rate", groupInput->executionRate);
	printf("%21s: %ld\n", "timestamp", groupInput->timestamp);
	//  AI Resource 階層項目
	printf("%21s: %05d\n", "resource id", resourceInput->resourceId);
	//  AI Resource 量測項目(電力數據資料)
	for(int i=0; i<10; i++) {
		printf("PhV phsA : %5d\n", resourceInput->powerInfo[i].phvA);
		printf("PhV phsB : %5d\n", resourceInput->powerInfo[i].phvB);
		printf("PhV phsC : %5d\n", resourceInput->powerInfo[i].phvC);
		printf("A phsA : %5d\n", resourceInput->powerInfo[i].currA);
		printf("A phsB : %5d\n", resourceInput->powerInfo[i].currB);
		printf("A phsC : %5d\n", resourceInput->powerInfo[i].currC);
		printf("Hz : %5d\n", resourceInput->powerInfo[i].freq);
		printf("totW : %5d\n", resourceInput->powerInfo[i].totW);
		printf("totVAr : %5d\n", resourceInput->powerInfo[i].totVAr);
		printf("totPF : %5d\n", resourceInput->powerInfo[i].totPF);
		printf("SOC : %5d\n", resourceInput->powerInfo[i].soc);
		printf("ESS State : %5d\n", resourceInput->powerInfo[i].essState);
		printf("Timestamp : %ld\n", resourceInput->powerInfo[i].timestamp);
	}
}

void updateIedModel(IedServer iedServer, GroupInput *groupInput, ResourceInput *resourceInput) {
	IedServer_lockDataModel(iedServer);

	Timestamp iecTimestamp;
	Timestamp_clearFlags(&iecTimestamp);
	Timestamp_setTimeInMilliseconds(&iecTimestamp, groupInput->timestamp);
	Timestamp_setLeapSecondKnown(&iecTimestamp, true);

	IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_G8477_DREGGIO01_Ind1_stVal, groupInput->serviceStart);
	IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_G8477_DREGGIO01_Ind1_t,  &iecTimestamp);
	IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_G8477_DREGGIO02_Ind1_stVal, groupInput->serviceStop);
	IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_G8477_DREGGIO02_Ind1_t, &iecTimestamp);
	IedServer_updateInt32AttributeValue(iedServer, IEDMODEL_G8477_GROGGIO_IntIn1_stVal, groupInput->providerId);
	IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_G8477_GROGGIO_IntIn1_t, &iecTimestamp);
	IedServer_updateInt32AttributeValue(iedServer, IEDMODEL_G8477_GROGGIO_IntIn2_stVal, groupInput->quotationId);
	IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_G8477_GROGGIO_IntIn2_t, &iecTimestamp);
	IedServer_updateInt32AttributeValue(iedServer, IEDMODEL_G8477_GROGGIO_IntIn3_stVal, groupInput->serviceCode);
	IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_G8477_GROGGIO_IntIn3_t, &iecTimestamp);
	IedServer_updateInt32AttributeValue(iedServer, IEDMODEL_G8477_GROMMXU01_TotW_mag_i, groupInput->supplyWatt);
	IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_G8477_GROMMXU01_TotW_t, &iecTimestamp);
	// see 'How to store a 64 bit integer in two 32 bit integers and convert back again' in stackoverflow
	IedServer_updateInt32AttributeValue(iedServer, IEDMODEL_G8477_GROGGIO_AnIn1_mag_i, (uint32_t)(groupInput->timestamp >> 32));
	IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_G8477_GROGGIO_AnIn1_t, &iecTimestamp);
	IedServer_updateInt32AttributeValue(iedServer, IEDMODEL_G8477_GROGGIO_AnIn2_mag_i, (uint32_t)(groupInput->timestamp));
	IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_G8477_GROGGIO_AnIn2_t, &iecTimestamp);
	IedServer_updateInt32AttributeValue(iedServer, IEDMODEL_G8477_GROGGIO_AnIn3_mag_i, groupInput->executionRate);
	IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_G8477_GROGGIO_AnIn3_t, &iecTimestamp);
	for(int i=0; i<10; i++) {
		IedServer_updateInt32AttributeValue(iedServer, groupRealPowerTsHighDaNames[i], (uint32_t)(groupInput->realPower[i].timestamp >> 32));
		IedServer_updateInt32AttributeValue(iedServer, groupRealPowerTsLowDaNames[i], (uint32_t)(groupInput->realPower[i].timestamp));
		IedServer_updateInt32AttributeValue(iedServer, groupRealPowerDaNames[i], groupInput->realPower[i].totalWatt);
	}

	// AI for 交易資源
	IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_R8623_GGIO01_IntIn1_stVal, resourceInput->providerId);
	IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_R8623_GGIO02_IntIn1_stVal, resourceInput->quotationId);
	IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_R8623_GGIO03_IntIn1_stVal, resourceInput->resourceId);
	IedServer_updateBooleanAttributeValue(iedServer, IEDMODEL_R8623_GGIO04_IntIn1_stVal, resourceInput->serviceCode);

	// 所有交易資源1-10秒電力數據資料
	for(int i=0; i<10; i++) {
		IedServer_updateInt32AttributeValue(iedServer, resourcePowerInfoPhvADaNames[i], resourceInput->powerInfo[i].phvA);
		IedServer_updateInt32AttributeValue(iedServer, resourcePowerInfoPhvBDaNames[i], resourceInput->powerInfo[i].phvB);
		IedServer_updateInt32AttributeValue(iedServer, resourcePowerInfoPhvCDaNames[i], resourceInput->powerInfo[i].phvC);
		IedServer_updateInt32AttributeValue(iedServer, resourcePowerInfoCurrADaNames[i], resourceInput->powerInfo[i].currA);
		IedServer_updateInt32AttributeValue(iedServer, resourcePowerInfoCurrBDaNames[i], resourceInput->powerInfo[i].currB);
		IedServer_updateInt32AttributeValue(iedServer, resourcePowerInfoCurrCDaNames[i], resourceInput->powerInfo[i].currC);
		IedServer_updateInt32AttributeValue(iedServer, resourcePowerInfoHzDaNames[i], resourceInput->powerInfo[i].freq);
		IedServer_updateInt32AttributeValue(iedServer, resourcePowerInfoTotWDaNames[i], resourceInput->powerInfo[i].totW);
		IedServer_updateInt32AttributeValue(iedServer, resourcePowerInfoTotVArDaNames[i], resourceInput->powerInfo[i].totVAr);
		IedServer_updateInt32AttributeValue(iedServer, resourcePowerInfoTotPFDaNames[i], resourceInput->powerInfo[i].totPF);
		IedServer_updateInt32AttributeValue(iedServer, resourcePowerInfoSocDaNames[i], resourceInput->powerInfo[i].soc);
		IedServer_updateBooleanAttributeValue(iedServer, resourcePowerInfoBatStatDaNames[i], resourceInput->powerInfo[i].essState);
		IedServer_updateInt32AttributeValue(iedServer, resourcePowerInfoTsHighDaNames[i], (uint32_t)(resourceInput->powerInfo[i].timestamp >> 32));
		IedServer_updateInt32AttributeValue(iedServer, resourcePowerInfoTsLowDaNames[i], (uint32_t)(resourceInput->powerInfo[i].timestamp));
	}

	IedServer_unlockDataModel(iedServer);
}

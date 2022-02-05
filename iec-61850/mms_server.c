/*
 * mms_server.c 參考libiec61850/examples/server_example_basic_io/server_example_basic_io.c.
 *
 *  Created on: 2022年2月3日
 *      Author: ken
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
	int totalWatt;
	unsigned long timestamp;
};
typedef struct GroupRealPowerStruc GroupRealPower;

// DI and AI of 報價代碼
struct GroupInputStruc {
	// 履行服務開始
	bool serviceStart;
	// 履行服務結束
	bool serviceStop;
	// 合格交易者代碼
	unsigned int providerId;
	// 報價代碼
	unsigned int quotationId;
	// 輔助服務商品
	unsigned char serviceCode;
	// 所有交易資源加總之瞬時輸出/入總實功率
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

DataAttribute *groupRealPowerTsHighDaNames[10] = {IEDMODEL_G8477_GROGGIO01_AnIn1_mag_i, IEDMODEL_G8477_GROGGIO02_AnIn1_mag_i, IEDMODEL_G8477_GROGGIO03_AnIn1_mag_i,
		IEDMODEL_G8477_GROGGIO04_AnIn1_mag_i, IEDMODEL_G8477_GROGGIO05_AnIn1_mag_i, IEDMODEL_G8477_GROGGIO06_AnIn1_mag_i,
		IEDMODEL_G8477_GROGGIO07_AnIn1_mag_i, IEDMODEL_G8477_GROGGIO08_AnIn1_mag_i, IEDMODEL_G8477_GROGGIO09_AnIn1_mag_i,
		IEDMODEL_G8477_GROGGIO10_AnIn1_mag_i};

DataAttribute *groupRealPowerTsLowDaNames[10] = {IEDMODEL_G8477_GROGGIO01_AnIn2_mag_i, IEDMODEL_G8477_GROGGIO02_AnIn2_mag_i, IEDMODEL_G8477_GROGGIO03_AnIn2_mag_i,
		IEDMODEL_G8477_GROGGIO04_AnIn2_mag_i, IEDMODEL_G8477_GROGGIO05_AnIn2_mag_i, IEDMODEL_G8477_GROGGIO06_AnIn2_mag_i,
		IEDMODEL_G8477_GROGGIO07_AnIn2_mag_i, IEDMODEL_G8477_GROGGIO08_AnIn2_mag_i, IEDMODEL_G8477_GROGGIO09_AnIn2_mag_i,
		IEDMODEL_G8477_GROGGIO10_AnIn2_mag_i};

DataAttribute *groupRealPowerDaNames[10] = {IEDMODEL_G8477_GROMMXU01_TotW_mag_i, IEDMODEL_G8477_GROMMXU02_TotW_mag_i, IEDMODEL_G8477_GROMMXU03_TotW_mag_i,
		IEDMODEL_G8477_GROMMXU04_TotW_mag_i, IEDMODEL_G8477_GROMMXU05_TotW_mag_i, IEDMODEL_G8477_GROMMXU06_TotW_mag_i,
		IEDMODEL_G8477_GROMMXU07_TotW_mag_i, IEDMODEL_G8477_GROMMXU08_TotW_mag_i, IEDMODEL_G8477_GROMMXU09_TotW_mag_i,
		IEDMODEL_G8477_GROMMXU10_TotW_mag_i};

// function prototypes
void simulate(GroupInput *groupInput);
void printOut(GroupInput *groupInput);
void updateIedModel(IedServer iedServer, GroupInput *groupInput);

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
		uint64_t timestamp = Hal_getTimeInMs();
		Timestamp iecTimestamp;
		Timestamp_clearFlags(&iecTimestamp);
		Timestamp_setTimeInMilliseconds(&iecTimestamp, timestamp);
		Timestamp_setLeapSecondKnown(&iecTimestamp, true);

		GroupInput *groupInput = malloc(sizeof(GroupInput));
		simulate(groupInput);
		updateIedModel(iedServer, groupInput);
		printOut(groupInput);
		free(groupInput);
		Thread_sleep(1000);
	}

	/* stop MMS server - close TCP server socket and all client sockets */
	IedServer_stop(iedServer);

	/* Cleanup - free all resources */
	IedServer_destroy(iedServer);

	return 0;
}

void simulate(GroupInput *groupInput) {
	groupInput->serviceStart = true;
	groupInput->serviceStop = false;
	groupInput->providerId = 84778623;
	groupInput->quotationId = 6163;
	groupInput->serviceCode = 0;
	groupInput->supplyWatt = 12000;
	groupInput->demandWatt = 10000;
	groupInput->timestamp = Hal_getTimeInMs();
	groupInput->executionRate = 8888;
	for(int i=0; i<10; i++) {
		GroupRealPower *groupRealPower = malloc(sizeof(GroupRealPower));
		groupRealPower->totalWatt = 10000 - 1000 * sin(2 * PI * (40 * i) / 360);
		groupRealPower->timestamp = groupInput->timestamp - (10 - i) * 1000;
		groupInput->realPower[i] = *groupRealPower;
	}
}

void printOut(GroupInput *groupInput) {
	printf("%21s: %d\n", "service start", groupInput->serviceStart);
	printf("%21s: %d\n", "service stop", groupInput->serviceStop);
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
}

void updateIedModel(IedServer iedServer, GroupInput *groupInput) {
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
    char *daName = "IEDMODEL_G8477_GROGGIO01_AnIn1_mag_i";
	for(int i=0; i<10; i++) {
		IedServer_updateInt32AttributeValue(iedServer, groupRealPowerTsHighDaNames[i], (uint32_t)(groupInput->realPower[i].timestamp >> 32));
		IedServer_updateInt32AttributeValue(iedServer, groupRealPowerTsLowDaNames[i], (uint32_t)(groupInput->realPower[i].timestamp));
		IedServer_updateInt32AttributeValue(iedServer, groupRealPowerDaNames[i], groupInput->realPower[i].totalWatt);
	}

	IedServer_unlockDataModel(iedServer);
}

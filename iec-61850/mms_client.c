/*
 * mms_client.c
 *
 *  Created on: 2022年2月4日
 *      Author: ken
 */

#include "iec61850_client.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include "hal_thread.h"
#include <sqlite3.h>

// AI報價代碼Report, report name is "urcb01", but generated data model name this report as "urcb0101"
#define AI_GROUP_REPORT_NAME  "HwacomAFCG08477/LLN0.RP.urcb0101"
// AI交易資源量測項目(電力數據資料)
#define AI_RESOURCE_REPORT_NAME "HwacomAFCR08623/LLN0.RP.urcb0201"

static int running = 0;

void sigint_handler(int signalId) {
	running = 0;
}

char IEC61850_REASON_NAMES[][20] = {"NOT_INCLUDED", "DATA_CHANGE", "QUALITY_CHANGE", "DATA_UPDATE",
                                    "INTEGRITY",
                                    "GI", "UNKNOWN"};



int getAiResourceDataArrayIdx(char *doName) {
	char *idxSlash = strchr(doName, '/');
	char lD2DA[50];
	strcpy(lD2DA, idxSlash + 8);
	int i = (int) strtol(lD2DA, NULL, 10);
	return i;
}

void saveToDB(char **sqlStatements, int n) {
	sqlite3 *db;
	char *errMsg = NULL;
	/* 開啟 database 檔 */
	if (sqlite3_open_v2("taipower.db", &db,
	SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL)) {
		return;
	}

	sqlite3_exec(db, "PRAGMA journal_mode=WAL;", 0, 0, &errMsg);
	for (int i=0; i<n; i++) {
		sqlite3_exec(db, sqlStatements[i], 0, 0, &errMsg);
	}

	/* 關閉 database */
	sqlite3_close(db);
}

/**
 * 將接收到的報價代碼AI資料寫入資料庫中.
 */
void saveAiGrpReportData(ClientReport report, LinkedList dataSetDirectory, MmsValue *dataSetValues) {
	unsigned int quotationId;
	unsigned int executionRate;
	unsigned long execRateTime1;
	unsigned long execRateTime2;
	unsigned int totW[10];
	unsigned long time1[10];
	unsigned long time2[10];

	if (!dataSetDirectory) {
		return;
	}

	int i;
	for (i = 0; i < LinkedList_size(dataSetDirectory); i++) {
		ReasonForInclusion reason = ClientReport_getReasonForInclusion(report, i);
		if (reason == IEC61850_REASON_NOT_INCLUDED) {
			continue;
		}

		char valBuffer[500];
		sprintf(valBuffer, "no value");

		if (dataSetValues) {
			MmsValue *value = MmsValue_getElement(dataSetValues, i);
			if (value) {
				MmsValue_printToBuffer(value, valBuffer, 500);
			}
		}

		LinkedList entry = LinkedList_get(dataSetDirectory, i);

		char *entryName = (char*) entry->data;
		int idx = getAiResourceDataArrayIdx(entryName) - 1;

		if (strstr(entryName, "IntIn2")) {
			quotationId = atoi(valBuffer);
		} else if (strstr(entryName, "TotW")) {
			totW[idx] = atoi(valBuffer);
		} else if (strstr(entryName, "GROGGIO.AnIn1")) {
			execRateTime1 = atoi(valBuffer);
		} else if (strstr(entryName, "GROGGIO.AnIn2")) {
			execRateTime2 = atoi(valBuffer);
		} else if (strstr(entryName, "GROGGIO.AnIn3")) {
			executionRate = atoi(valBuffer);
		} else if (strstr(entryName, "AnIn1")) {
			time1[idx] = atoi(valBuffer);
		} else if (strstr(entryName, "AnIn2")) {
			time2[idx] = atoi(valBuffer);
		}

	} //end of for(i = 0; i < LinkedList_size(dataSetDirectory); i++)

	char sqlStatement[500];
	char *sqlStatements[10];
	char *sqlStrFormat1 = "INSERT INTO group_performance VALUES(NULL, %d, %d, %d)";
	char *sqlStrFormat2 = "INSERT INTO group_real_power VALUES(NULL, 8477, %d, %d)";

	sprintf(sqlStatement, sqlStrFormat1, quotationId, executionRate, (uint64_t) execRateTime1 << 32 | execRateTime2);
	sqlStatements[0] = sqlStatement;
	saveToDB(sqlStatements, 1);

	uint64_t ts;
	for (i = 0; i < 10; i++) {
		ts = ((uint64_t) time1[i]) << 32 | time2[i];
		sprintf(sqlStatement, sqlStrFormat2, totW[i], ts);
		sqlStatements[i] = sqlStatement;
	}
	saveToDB(sqlStatements, 10);
}

/**
 * 將接收到的交易資源AI資料寫入資料庫中.
 */
void saveAiResReportData(ClientReport report, LinkedList dataSetDirectory, MmsValue *dataSetValues) {
	unsigned int phvA[10];
	unsigned int phvB[10];
	unsigned int phvC[10];
	unsigned int currA[10];
	unsigned int currB[10];
	unsigned int currC[10];
	unsigned int freq[10];
	unsigned int totW[10];
	unsigned int totVAr[10];
	unsigned int totPF[10];
	unsigned int soc[10];
	bool essState[10];
	unsigned long time1[10];
	unsigned long time2[10];

	if (!dataSetDirectory) {
		return;
	}

	int i;
	// LinkedList_size(dataSetDirectory) = how many FCDA in a dataset
	for (i = 0; i < LinkedList_size(dataSetDirectory); i++) {
		ReasonForInclusion reason = ClientReport_getReasonForInclusion(report, i);
		if (reason == IEC61850_REASON_NOT_INCLUDED) {
			continue;
		}

		char valBuffer[500];
		sprintf(valBuffer, "no value");

		if (dataSetValues) {
			MmsValue *value = MmsValue_getElement(dataSetValues, i);
			// 這裡是把MmsValue轉成String,再由String透過atoi轉數字儲存到DB,不知道有沒有更好的方式?
			if (value) {
				MmsValue_printToBuffer(value, valBuffer, 500);
			}
		}

		LinkedList entry = LinkedList_get(dataSetDirectory, i);

		// ex: HwacomAFCR08623/DREMMXU01.PhV.phsA.cVal.mag.i[MX]
		char *entryName = (char*) entry->data;
		int idx = getAiResourceDataArrayIdx(entryName) - 1;

		if (strstr(entryName, "PhV.phsA")) {
			phvA[idx] = atoi(valBuffer);
		} else if (strstr(entryName, "PhV.phsB")) {
			phvB[idx] = atoi(valBuffer);
		} else if (strstr(entryName, "PhV.phsC")) {
			phvC[idx] = atoi(valBuffer);
		} else if (strstr(entryName, "A.phsA")) {
			currA[idx] = atoi(valBuffer);
		} else if (strstr(entryName, "A.phsB")) {
			currB[idx] = atoi(valBuffer);
		} else if (strstr(entryName, "A.phsC")) {
			currC[idx] = atoi(valBuffer);
		} else if (strstr(entryName, "Hz")) {
			freq[idx] = atoi(valBuffer);
		} else if (strstr(entryName, "TotW")) {
			totW[idx] = atoi(valBuffer);
		} else if (strstr(entryName, "TotVAr")) {
			totVAr[idx] = atoi(valBuffer);
		} else if (strstr(entryName, "TotPF")) {
			totPF[idx] = atoi(valBuffer);
		} else if (strstr(entryName, "InBatV")) {
			soc[idx] = atoi(valBuffer);
		} else if (strstr(entryName, "BatSt")) {
			essState[idx] = atoi(valBuffer);
		} else if (strstr(entryName, "AnIn1")) {
			time1[idx] = atoi(valBuffer);
		} else if (strstr(entryName, "AnIn2")) {
			time2[idx] = atoi(valBuffer);
		}

	} //end of for(i = 0; i < LinkedList_size(dataSetDirectory); i++)

	char sqlStatement[500];
	char *sqlStrFormat =
			"INSERT INTO resource_measurement VALUES(NULL, 8623, %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)";
	char *sqlStatements[10];

	uint64_t ts;
	for (i = 0; i < 10; i++) {
		ts = ((uint64_t) time1[i]) << 32 | time2[i];
		sprintf(sqlStatement, sqlStrFormat, phvA[i], phvB[i], phvC[i], currA[i],
				currB[i], currC[i], freq[i], totW[i], totVAr[i], totPF[i],
				soc[i], essState[i], ts);
		sqlStatements[i] = sqlStatement;
	}
	saveToDB(sqlStatements, 10);
}

/**
 * 負責處理接收到的報表資料的回呼函式.
 */
void reportCallbackFunction(void *parameter, ClientReport report) {
  LinkedList dataSetDirectory = (LinkedList) parameter;
  MmsValue *dataSetValues = ClientReport_getDataSetValues(report);

  // ex: HwacomAFCG08477/LLN0.RP.urcb0101(AI group) , HwacomAFCR08623/LLN0.RP.urcb0201(AI Resource)
  char *rcbName = ClientReport_getRcbReference(report);
  printf("received report for RCB: %s\n", rcbName);

  if (strstr(rcbName, "R08623")) {
	  saveAiResReportData(report, dataSetDirectory, dataSetValues);
  } else if (strstr(rcbName, "G08477")) {
	  saveAiGrpReportData(report, dataSetDirectory, dataSetValues);
  }
}

/**
 * 主程式,連接MMS Server後,訂閱報價代碼與交易資源AI資料報表.
 */
int main(int argc, char **argv) {
	char *hostname;
	int tcpPort = 102;

	if (argc > 1)
		hostname = argv[1];
	else
		hostname = "localhost";

	if (argc > 2)
		tcpPort = atoi(argv[2]);

	running = 1;

	signal(SIGINT, sigint_handler);

	IedClientError error;
	IedConnection con = IedConnection_create();

	IedConnection_connect(con, &error, hostname, tcpPort);
	printf("%s\n", "connect to mms server");

	if (error != IED_ERROR_OK) {
	    goto exit_error;
	}

	ClientReportControlBlock rcbAiGrp = NULL;
	ClientReportControlBlock rcbAiRes = NULL;

	/* Read RCB values */
	rcbAiGrp = IedConnection_getRCBValues(con, &error, AI_GROUP_REPORT_NAME, NULL);
	rcbAiRes = IedConnection_getRCBValues(con, &error, AI_RESOURCE_REPORT_NAME, NULL);

	if (error != IED_ERROR_OK) {
		printf("getRCBValues service error!\n");
		goto exit_error;
	}

	const char *dataSetAiGrpName = ClientReportControlBlock_getDataSetReference(rcbAiGrp);
	const char *dataSetAiResName = ClientReportControlBlock_getDataSetReference(rcbAiRes);
	LinkedList dataSetAiGrpDirectory = IedConnection_getDataSetDirectory(con, &error, dataSetAiGrpName, NULL);
	LinkedList dataSetAiResDirectory = IedConnection_getDataSetDirectory(con, &error, dataSetAiResName, NULL);

	/* Configure the report receiver */
	IedConnection_installReportHandler(con, AI_GROUP_REPORT_NAME, ClientReportControlBlock_getRptId(rcbAiGrp), reportCallbackFunction,
				(void*) dataSetAiGrpDirectory);
	IedConnection_installReportHandler(con, AI_RESOURCE_REPORT_NAME, ClientReportControlBlock_getRptId(rcbAiRes), reportCallbackFunction,
			(void*) dataSetAiResDirectory);

	ClientReportControlBlock_setRptEna(rcbAiGrp, true);
	ClientReportControlBlock_setRptEna(rcbAiRes, true);
	IedConnection_setRCBValues(con, &error, rcbAiGrp, RCB_ELEMENT_RPT_ENA, true);
	IedConnection_setRCBValues(con, &error, rcbAiRes, RCB_ELEMENT_RPT_ENA, true);

	if (error != IED_ERROR_OK) {
		printf("setRCBValues service error!\n");
		goto exit_error;
	}

	Thread_sleep(1000);

	while (running) {
		Thread_sleep(10);

		IedConnectionState conState = IedConnection_getState(con);

		if (conState != IED_STATE_CONNECTED) {
			printf("Connection closed by server!\n");
			running = 0;
		}
	}

	/* disable reporting */
	ClientReportControlBlock_setRptEna(rcbAiGrp, false);
	ClientReportControlBlock_setRptEna(rcbAiRes, false);
	IedConnection_setRCBValues(con, &error, rcbAiGrp, RCB_ELEMENT_RPT_ENA, true);
	IedConnection_setRCBValues(con, &error, rcbAiRes, RCB_ELEMENT_RPT_ENA, true);

exit_error:

	IedConnection_close(con);

	if (rcbAiGrp)
			ClientReportControlBlock_destroy(rcbAiGrp);
	if (rcbAiRes)
		ClientReportControlBlock_destroy(rcbAiRes);

	if (dataSetAiGrpDirectory)
		LinkedList_destroy(dataSetAiGrpDirectory);
	if (dataSetAiResDirectory)
		LinkedList_destroy(dataSetAiResDirectory);


	    IedConnection_destroy(con);
	    return 0;
}

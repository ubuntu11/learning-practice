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
#define AI_GROUP_REPORT_NAME  "HwacomAFCG8477/LLN0.RP.urcb0101"
// AI交易資源量測項目(電力數據資料)
#define AI_RESOURCE_REPORT_NAME "HwacomAFCR8623/LLN0.RP.urcb0201"

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

void reportCallbackFunction(void *parameter, ClientReport report) {
  LinkedList dataSetDirectory = (LinkedList) parameter;
  MmsValue *dataSetValues = ClientReport_getDataSetValues(report);

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

  // ex: HwacomAFCR8623/LLN0.RP.urcb0201
  printf("received report for RCB: %s\n", ClientReport_getRcbReference(report));

  if (dataSetDirectory) {
    int i;
    // LinkedList_size(dataSetDirectory) = how many FCDA in a dataset
    for (i = 0; i < LinkedList_size(dataSetDirectory); i++) {
      ReasonForInclusion reason = ClientReport_getReasonForInclusion(report, i);
      if (reason != IEC61850_REASON_NOT_INCLUDED) {

        char valBuffer[500];
        sprintf(valBuffer, "no value");

        if (dataSetValues) {
          MmsValue *value = MmsValue_getElement(dataSetValues, i);
          // create a string representation of the MmsValue object in the provided buffer,
          // for debugging purposes only
          if (value) {
            MmsValue_printToBuffer(value, valBuffer, 500);
          }
        }

        LinkedList entry = LinkedList_get(dataSetDirectory, i);

        // ex: HwacomAFCR8623/DREMMXU01.PhV.phsA.cVal.mag.i[MX]
        char *entryName = (char *) entry->data;
        int idx = getAiResourceDataArrayIdx(entryName) - 1;

        if (strstr(entryName, "PhV.phsA")) {
        	phvA[idx] = atoi(valBuffer);
        } else if(strstr(entryName, "PhV.phsB")) {
        	phvB[idx] = atoi(valBuffer);
        } else if(strstr(entryName, "PhV.phsC")) {
        	phvC[idx] = atoi(valBuffer);
        } else if(strstr(entryName, "A.phsA")) {
        	currA[idx] = atoi(valBuffer);
        } else if(strstr(entryName, "A.phsB")) {
        	currB[idx] = atoi(valBuffer);
        } else if(strstr(entryName, "A.phsC")) {
        	currC[idx] = atoi(valBuffer);
        } else if(strstr(entryName, "Hz")) {
        	freq[idx] = atoi(valBuffer);
        } else if(strstr(entryName, "TotW")) {
        	totW[idx] = atoi(valBuffer);
        } else if(strstr(entryName, "TotVAr")) {
        	totVAr[idx] = atoi(valBuffer);
        } else if(strstr(entryName, "TotPF")) {
        	totPF[idx] = atoi(valBuffer);
        } else if(strstr(entryName, "InBatV")) {
        	soc[idx] = atoi(valBuffer);
        } else if(strstr(entryName, "BatSt")) {
        	essState[idx] = atoi(valBuffer);
        } else if(strstr(entryName, "AnIn1")) {
        	time1[idx] = atoi(valBuffer);
        } else if(strstr(entryName, "AnIn2")) {
        	time2[idx] = atoi(valBuffer);
        }

//        if (ClientReport_hasTimestamp(report)) {
//          printf("%li  %s (included for reason %s): %s\n", ClientReport_getTimestamp(report),
//                 entryName,
//                 IEC61850_REASON_NAMES[reason], valBuffer);
//        } else {
//          printf("%s (included for reason %s): %s\n", entryName, IEC61850_REASON_NAMES[reason],
//                 valBuffer);
//        }

      }
    } //end of for(i = 0; i < LinkedList_size(dataSetDirectory); i++)

    char sqlStatement[500];
    char *sqlStrFormat = "INSERT INTO resource_measurement VALUES(NULL, 8623, %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)";

    sqlite3 *db;
    char *errMsg = NULL;
	/* 開啟 database 檔 */
	if (sqlite3_open_v2("taipower.db", &db,
			SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL)) {
		return;
	}

	uint64_t ts;
    for (i = 0; i < 10; i++) {
        ts = ((uint64_t) time1[i]) << 32 | time2[i];
    	sprintf(sqlStatement, sqlStrFormat, phvA[i], phvB[i], phvC[i], currA[i], currB[i], currC[i], freq[i], totW[i], totVAr[i], totPF[i], soc[i], essState[i], ts);
    	// printf("sql statement= %s\n", sqlStatement);
    	sqlite3_exec(db, sqlStatement, 0, 0, &errMsg);
    }

    /* 關閉 database */
    sqlite3_close(db);
  }
}

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

	ClientReportControlBlock rcb = NULL;

	/* Read RCB values */
	rcb = IedConnection_getRCBValues(con, &error, AI_RESOURCE_REPORT_NAME, NULL);

	if (error != IED_ERROR_OK) {
		printf("getRCBValues service error!\n");
		goto exit_error;
	}

	const char *dataSetName = ClientReportControlBlock_getDataSetReference(rcb);
	LinkedList dataSetDirectory = IedConnection_getDataSetDirectory(con, &error, dataSetName, NULL);

	/* Configure the report receiver */
	IedConnection_installReportHandler(con, AI_RESOURCE_REPORT_NAME, ClientReportControlBlock_getRptId(rcb), reportCallbackFunction,
			(void*) dataSetDirectory);

	ClientReportControlBlock_setRptEna(rcb, true);
	IedConnection_setRCBValues(con, &error, rcb, RCB_ELEMENT_RPT_ENA, true);

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
	ClientReportControlBlock_setRptEna(rcb, false);
	IedConnection_setRCBValues(con, &error, rcb, RCB_ELEMENT_RPT_ENA, true);

exit_error:

	IedConnection_close(con);

	if (rcb)
		ClientReportControlBlock_destroy(rcb);

	if (dataSetDirectory)
		LinkedList_destroy(dataSetDirectory);


	    IedConnection_destroy(con);
	    return 0;
}

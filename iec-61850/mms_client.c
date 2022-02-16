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

void reportCallbackFunction(void *parameter, ClientReport report) {
  LinkedList dataSetDirectory = (LinkedList) parameter;
  MmsValue *dataSetValues = ClientReport_getDataSetValues(report);

  printf("received report for RCB: %s\n", ClientReport_getRcbReference(report));

  if (dataSetDirectory) {
    int i;
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

        char *entryName = (char *) entry->data;

        if (ClientReport_hasTimestamp(report)) {
          printf("%li  %s (included for reason %s): %s\n", ClientReport_getTimestamp(report),
                 entryName,
                 IEC61850_REASON_NAMES[reason], valBuffer);
        } else {
          printf("%s (included for reason %s): %s\n", entryName, IEC61850_REASON_NAMES[reason],
                 valBuffer);
        }

      }
    }
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

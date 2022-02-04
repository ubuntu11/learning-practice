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

	if (error == IED_ERROR_OK) {

	        ClientReportControlBlock rcb = NULL;

	        if (error != IED_ERROR_OK) {
	            printf("Reading data set directory failed!\n");
	        //    goto exit_error;
	        }

	        /* Read RCB values */
	        rcb = IedConnection_getRCBValues(con, &error, "TEMPLATEDevice1/LLN0.RP.urcb01", NULL);

	        if (error != IED_ERROR_OK) {
	            printf("getRCBValues service error!\n");
	            goto exit_error;
	        }

	        const char *dataSetName = ClientReportControlBlock_getDataSetReference(rcb);
	        LinkedList dataSetDirectory = IedConnection_getDataSetDirectory(con, &error, dataSetName, NULL);

	        /* prepare the parameters of the RCB */
//	        ClientReportControlBlock_setResv(rcb, true);
//	        ClientReportControlBlock_setTrgOps(rcb, TRG_OPT_DATA_CHANGED | TRG_OPT_QUALITY_CHANGED | TRG_OPT_GI);
//	        ClientReportControlBlock_setDataSetReference(rcb, "simpleIOGenericIO/LLN0$Events"); /* NOTE the "$" instead of "." ! */
//	        ClientReportControlBlock_setRptEna(rcb, true);
//	        ClientReportControlBlock_setGI(rcb, true);

	        /* Configure the report receiver */
	        IedConnection_installReportHandler(con, "TEMPLATEDevice1/LLN0.RP.urcb01", ClientReportControlBlock_getRptId(rcb), reportCallbackFunction,
	                (void*) dataSetDirectory);

	        /* Write RCB parameters and enable report */
	        IedConnection_setRCBValues(con, &error, rcb, RCB_ELEMENT_RESV | RCB_ELEMENT_DATSET | RCB_ELEMENT_TRG_OPS | RCB_ELEMENT_RPT_ENA | RCB_ELEMENT_GI, true);

	        if (error != IED_ERROR_OK) {
	            printf("setRCBValues service error!\n");
	            goto exit_error;
	        }

	        Thread_sleep(1000);

	        /* Trigger GI Report */
	        ClientReportControlBlock_setGI(rcb, true);
	        IedConnection_setRCBValues(con, &error, rcb, RCB_ELEMENT_GI, true);

	        if (error != IED_ERROR_OK) {
	            printf("Error triggering a GI report (code: %i)\n", error);
	        }

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
	    }
	    else {
	        printf("Failed to connect to %s:%i\n", hostname, tcpPort);
	    }

	    IedConnection_destroy(con);
	    return 0;
}

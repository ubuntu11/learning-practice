/*
 * ems.c 模擬EMS對儲能系統下達控制指令,主要是為了在演練履行服務開始及履行服務結束.
 *
 *  Created on: 2022年2月11日
 *      Author: ken
 */

#include "iec61850_client.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "hal_thread.h"

int main(int argc, char** argv) {

    char* hostname;
    int tcpPort = 102;

    if (argc > 1)
        hostname = argv[1];
    else
        hostname = "localhost";

    if (argc > 2)
        tcpPort = atoi(argv[2]);

    IedClientError error;

    IedConnection con = IedConnection_create();

    IedConnection_connect(con, &error, hostname, tcpPort);

    if (error == IED_ERROR_OK) {

    }
}


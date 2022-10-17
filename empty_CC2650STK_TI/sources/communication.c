/*
 * communication.c
 *
 *  Created on: 12 Oct 2022
 *      Author: 30v
 */
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/System.h>

#include "communication.h"

#define STACKSIZE 512
char taskStack[STACKSIZE];

void initCommunicationTask(void) {
    Task_Params taskParams;
    Task_Handle taskHandle;

    Task_Params_init(&taskParams);
    taskParams.stackSize = STACKSIZE;
    taskParams.stack = taskStack;
    taskParams.priority = 2;

    taskHandle = Task_create((Task_FuncPtr) communicationTaskFxn, &taskParams, NULL);
    if (taskHandle == NULL) {
        System_abort("Communication sensor task creation failed\n");
    }
}

void communicationTaskFxn(UArg arg0, UArg arg1) {
    Init6LoWPAN();
    if(StartReceive6LoWPAN() != true) {
       System_abort("Wireless receive start failed");
    }
    char receivedPayload[80];
    uint16_t senderAddr;
    while (1) {

        if (GetRXFlag()){
            memset(receivedPayload, 0 ,80);
            Receive6LoWPAN(&senderAddr, receivedPayload, 80);
        }

        if (commandToSend) {
            sendCommand(commandToSend);
            StartReceive6LoWPAN();
            commandToSend = EMPTY;
        }
        Task_sleep(1000000 / Clock_tickPeriod);
    }

}

void sendCommand(command commandToSend) {
    uint16_t address = GetAddr6LoWPAN();
    char payload[11] = {'\0'};
    formatPayload(payload, commandToSend);
    System_printf(payload);
    System_flush();
    Send6LoWPAN(address, (uint8_t*) payload, strlen(payload));
}

void formatPayload(char* payload, command commandToSend) {
    strcat(payload, getCommandAsStr(commandToSend));
    strcat(payload, ":");
    strcat(payload, "1");
}



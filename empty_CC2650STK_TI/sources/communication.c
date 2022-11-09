/*
 * communication.c
 *
 *  Created on: 12 Oct 2022
 *      Author: 30v
 */
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/System.h>

#include <stdio.h>

#include "communication.h"
#include "tamagotchiState.h"

#define MAX_LEN 80
#define OWN_ID 19

#define STACKSIZE 2048
static char taskStack[STACKSIZE];

void initCommunicationTask(void) {
    Task_Params taskParams;
    Task_Handle taskHandle;

    Task_Params_init(&taskParams);
    taskParams.stackSize = STACKSIZE;
    taskParams.stack = taskStack;
    taskParams.priority = 2;

    taskHandle = Task_create((Task_FuncPtr) communicationTaskFxn, &taskParams, NULL);
    if (taskHandle == NULL) {
        System_abort("Communication task creation failed\n");
    }
}

void communicationTaskFxn(UArg arg0, UArg arg1) {
    Init6LoWPAN();
    if(StartReceive6LoWPAN() != true) {
       System_abort("Wireless receive start failed");
    }
    char receivedPayload[MAX_LEN];
    uint16_t senderAddr;
    while (1) {

        if (GetRXFlag()){
            memset(receivedPayload, 0 , MAX_LEN);
            Receive6LoWPAN(&senderAddr, receivedPayload, MAX_LEN);
            handleReceivedMessage(receivedPayload);
        }

        if (commandToSend) {
            sendCommand(commandToSend);
            StartReceive6LoWPAN();
            commandToSend = EMPTY_COMMAND;
        }
        Task_sleep(1000000 / Clock_tickPeriod);
    }

}

void sendCommand(command commandToSend) {
    uint16_t address = GetAddr6LoWPAN();
    char payload[11] = {'\0'};
    formatPayload(payload, commandToSend);
    Send6LoWPAN(address, (uint8_t*) payload, strlen(payload));
}

void formatPayload(char* payload, command commandToSend) {
    strcat(payload, getCommandAsStr(commandToSend));
    strcat(payload, ":");
    strcat(payload, "1");
}

void handleReceivedMessage(char* receivedPayload) {
    char* id = strtok(receivedPayload, ",");
    char* command = strtok(NULL, ",");
    static char ownId[5];
    sprintf(ownId, "%d", OWN_ID);

    if (id && command) {
        if (!strcmp(id, ownId) && !strcmp(command, "BEEP")) {
            tState = CRITICAL;
        }
    }
}



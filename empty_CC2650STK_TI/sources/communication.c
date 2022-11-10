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
#include "stateMachine.h"
#include "led.h"

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
        if (programState == COMMUNICATION) {
            changeLedState(led2Handle);
            if (GetRXFlag()){
                memset(receivedPayload, 0 , MAX_LEN);
                Receive6LoWPAN(&senderAddr, receivedPayload, MAX_LEN);
                handleReceivedMessage(receivedPayload);
            }

            sendCommands();
            StartReceive6LoWPAN();
            memset(commandsToSend, EMPTY_COMMAND, sizeof(commandsToSend));
            programState = WAITING;
        }
        Task_sleep(1000000 / Clock_tickPeriod);
    }

}

void sendCommands() {
    uint16_t address = GetAddr6LoWPAN();
    char payload[MAX_LEN];

    for (size_t i = 0; i < COMM_INTERVAL; ++i) {
        if (commandsToSend[i] == EMPTY_COMMAND) {
            continue;
        }
        memset(payload, 0, MAX_LEN);
        formatPayload(payload, commandsToSend[i]);
        System_printf(payload);
        System_flush();
        Send6LoWPAN(address, (uint8_t*) payload, strlen(payload));
    }
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



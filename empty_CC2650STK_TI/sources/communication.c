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
#define OWN_ID 2019

#define STACKSIZE 2048
static char taskStack[STACKSIZE];

void initCommunicationTask(void) {
    Task_Params taskParams;
    Task_Handle taskHandle;

    Task_Params_init(&taskParams);
    taskParams.stackSize = STACKSIZE;
    taskParams.stack = taskStack;
    taskParams.priority = 1;

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
        changeLedState(led2Handle);
        if (GetRXFlag()){
            memset(receivedPayload, 0 , MAX_LEN);
            Receive6LoWPAN(&senderAddr, receivedPayload, MAX_LEN);
            handleReceivedMessage(receivedPayload);
        }
        if (programState == COMMUNICATION) {
            sendCommands();
            StartReceive6LoWPAN();
            memset(&commandsToSend, 0, sizeof(commandsToSend));
            programState = WAITING;
        }
    }

}

void sendCommands() {
    uint16_t address = GetAddr6LoWPAN();
    char payload[MAX_LEN] = {'\0'};

    formatPayload(payload);

    Send6LoWPAN(address, (uint8_t*) payload, strlen(payload));
}

void formatPayload(char* payload) {
    appendFormattedCommand(payload, EAT, commandsToSend.eatAmount);
    appendFormattedCommand(payload, PET, commandsToSend.petAmount);
    appendFormattedCommand(payload, EXERCISE, commandsToSend.exerciseAmount);
    appendFormattedMessage(payload, commandsToSend.msg1ToSend, 1);
    appendFormattedMessage(payload, commandsToSend.msg2ToSend, 2);
}

void appendFormattedCommand(char* payload, command commandToSend, uint8_t amountOfCommands) {
    if (amountOfCommands == 0) {
        return;
    }
    if (payload[0] != '\0') {
        strcat(payload, ",");
    }
    strcat(payload, getCommandAsStr(commandToSend));
    strcat(payload, ":");
    char amountOfCommandsStr[2];
    sprintf(amountOfCommandsStr, "%d", amountOfCommands);
    strcat(payload, amountOfCommandsStr);
}

void appendFormattedMessage(char* payload, customMsg msgToSend, uint8_t msgNum) {
    if (msgToSend == EMPTY_MSG) {
        return;
    }
    if (payload[0] != '\0') {
        strcat(payload, ",");
    }
    char msgNumStr[2];
    sprintf(msgNumStr, "%d", msgNum);
    strcat(payload, "MSG");
    strcat(payload, msgNumStr);
    strcat(payload, ":");
    strcat(payload, getCustomMsgAsStr(msgToSend));
}

void handleReceivedMessage(char* receivedPayload) {
    char* id = strtok(receivedPayload, ",");
    char* command = strtok(NULL, ",");
    char commandStartsWith[5];
    strncpy(commandStartsWith, command, 4);
    commandStartsWith[4] = '\0';
    char ownId[5];
    sprintf(ownId, "%d", OWN_ID);

    if (!strcmp(id, ownId) && !strcmp(commandStartsWith, "BEEP")) {
        tState = CRITICAL;
    }


}



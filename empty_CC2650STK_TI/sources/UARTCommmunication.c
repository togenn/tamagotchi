/*
 * UARTCommmunication.c
 *
 *  Created on: 21 Oct 2022
 *      Author: peral
 */

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/System.h>
#include <stdio.h>
#include <stdbool.h>
#include "Board.h"

#include "UARTCommunication.h"
#include "communication.h"
#include "tamagotchiState.h"


#define MAX_LEN 80
#define OWN_ID 2019

#define STACKSIZE 2048
static char taskStack[STACKSIZE];

bool dataReceived = false;

static void UARTWriteCallback(UART_Handle handle, void *txBuf, size_t size) {

}

static void UARTReadCallback(UART_Handle handle, void *rxBuf, size_t size) {
    dataReceived = true;
    UART_read(handle, rxBuf, MAX_LEN);
}

void initUARTCommTask(void) {
    Task_Params taskParams;
    Task_Handle taskHandle;

    Task_Params_init(&taskParams);
    taskParams.stackSize = STACKSIZE;
    taskParams.stack = taskStack;
    taskParams.priority = 1;

    taskHandle = Task_create((Task_FuncPtr) UARTCommTaskFxn, &taskParams, NULL);
    if (taskHandle == NULL) {
        System_abort("UART communication task creation failed\n");
    }
}

void initUART(UART_Handle* handle) {
    UART_Params params;
    UART_Params_init(&params);
    params.readMode = UART_MODE_CALLBACK;
    params.readCallback = UARTReadCallback;
    params.writeMode = UART_MODE_CALLBACK;
    params.writeCallback = UARTWriteCallback;
    params.baudRate = 9600;

    UART_init();
    *handle = UART_open(Board_UART, &params);

}

void UARTCommTaskFxn(UArg arg0, UArg arg1) {
    UART_Handle handle;

    initUART(&handle);
    char receivedPayload[MAX_LEN];
    UART_read(handle, receivedPayload, MAX_LEN);

    while (1) {

        if (dataReceived) {
            handleReceivedMessage(receivedPayload);
            dataReceived= false;
        }

        if (programState == COMMUNICATION) {
            sendCommandsUART(&handle);
            programState = UPDATE_UI;
        }
    }

}

void formatUARTPayload(char* payload) {
    char idStr[9] = {'\0'};
    sprintf(idStr, "id:%d", OWN_ID);
    strcat(payload, idStr);
    formatPayload(payload);
}

void sendCommandsUART(UART_Handle* handle) {
    static char payload[MAX_LEN];
    memset(payload, '\0', MAX_LEN);
    formatUARTPayload(payload);
    if (payload[7] != '\0') {
        UART_write(*handle, payload, MAX_LEN);
    }
}




/*
 * UARTCommmunication.c
 *
 *  Created on: 21 Oct 2022
 *      Author: peral
 */

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/System.h>
#include "Board.h"

#include <stdio.h>

#include "UARTCommunication.h"
#include "communication.h"
#include "tamagotchiState.h"

#define MAX_LEN 80
#define OWN_ID 19

#define STACKSIZE 2048
static char taskStack[STACKSIZE];

static void UARTWriteCallback(UART_Handle handle, void *txBuf, size_t size) {

}

static void UARTReadCallback(UART_Handle handle, void *rxBuf, size_t size) {

}

void initUARTCommTask(void) {
    Task_Params taskParams;
    Task_Handle taskHandle;

    Task_Params_init(&taskParams);
    taskParams.stackSize = STACKSIZE;
    taskParams.stack = taskStack;
    taskParams.priority = 2;

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
    while (1) {

        if (UART_read(handle, receivedPayload, MAX_LEN)){
            handleReceivedMessage(receivedPayload);
            memset(receivedPayload, 0 , MAX_LEN);
        }

        commandToSend = EAT;
        if (commandToSend) {
            sendCommandUART(&handle, commandToSend);
            commandToSend = EMPTY;
        }
        Task_sleep(1000000 / Clock_tickPeriod);
    }

}

void sendCommandUART(UART_Handle* handle, command commandToSend) {
    char payload[11] = {'\0'};
    formatPayload(payload, commandToSend);
    UART_write(*handle, payload, 11);
}




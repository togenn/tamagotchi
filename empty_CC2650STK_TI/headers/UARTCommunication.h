/*
 * UARTCommunication.h
 *
 *  Created on: 21 Oct 2022
 *      Author: peral
 */

#ifndef HEADERS_UARTCOMMUNICATION_H_
#define HEADERS_UARTCOMMUNICATION_H_

#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC26XX.h>

#include "commands.h"

void initUARTCommTask(void);

void initUART(UART_Handle* handle);
void UARTCommTaskFxn(UArg arg0, UArg arg1);
void sendCommandUART(UART_Handle* handle, command commandToSend);
void formatUARTPayload(char* payload, command commandToSend);


#endif /* HEADERS_UARTCOMMUNICATION_H_ */

/*
 * communication.h
 *
 *  Created on: 12 Oct 2022
 *      Author: Toni
 */

#ifndef HEADERS_COMMUNICATION_H_
#define HEADERS_COMMUNICATION_H_

#include <xdc/std.h>
#include <comm_lib.h>

#include "commands.h"

void initCommunicationTask(void);
void formatPayload(char* payload);
void appendFormattedCommand(char* payload, command commandToSend, uint8_t amountOfCommands);
void appendFormattedMessage(char* payload, customMsg msgToSend, uint8_t msgNum);
void handleReceivedMessage(char* receivedPayload);

void communicationTaskFxn(UArg arg0, UArg arg1);
void sendCommands();


#endif /* HEADERS_COMMUNICATION_H_ */

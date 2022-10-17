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

void communicationTaskFxn(UArg arg0, UArg arg1);
void sendCommand(command commandToSend);
void formatPayload(char* payload, command commandToSend);


#endif /* HEADERS_COMMUNICATION_H_ */

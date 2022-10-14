/*
 * communication.c
 *
 *  Created on: 12 Oct 2022
 *      Author: 30v
 */
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

#include "communication.h"

void communicationTaskFxn(UArg arg0, UArg arg1) {
    Init6LoWPAN();
    StartReceive6LoWPAN();

    while (1) {
        if (commandToSend) {
            sendCommand(commandToSend);
            commandToSend = EMPTY;
        }
        Task_sleep(1000000 / Clock_tickPeriod);
    }

}

void sendCommand(command commandToSend) {
    uint16_t address = GetAddr6LoWPAN();
    char payload[11];
    formatPayload(payload, commandToSend);
    Send6LoWPAN(address, (uint8_t*) payload, strlen(payload));
}

void formatPayload(char* payload, command commandToSend) {
    strcat(payload, getCommandAsStr(commandToSend));
    strcat(payload, ":");
    strcat(payload, "1");
}



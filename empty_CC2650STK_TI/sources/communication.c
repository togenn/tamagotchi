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
    static uint16_t address = GetAddr6LoWPAN();
    Send6LoWPAN(address, uint8_t *ptr_Payload, uint8_t u8_length);
}



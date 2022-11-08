/* C Standard library */
#include <stdio.h>
#include <stdlib.h>

/* XDCtools files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/UART.h>

/* Board Header files */
#include "Board.h"
#include "wireless/comm_lib.h"
#include "sensors/opt3001.h"

#include "notePlayer.h"
#include "stateMachine.h"
#include "accelData.h"
#include "communication.h"
#include "UARTCommunication.h"
#include "led.h"
#include "tamagotchiState.h"


state programState = WAITING;
tamagotchiState tState = OK;

int main(void) {

    Board_initGeneral();

    Board_initI2C();

    initLed();

    initAccelSensorTask();
    //initCommunicationTask();
    //initBuzzerTask();
    //initUARTCommTask();

    /* Sanity check */
    System_printf("Hello world!\n");
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}

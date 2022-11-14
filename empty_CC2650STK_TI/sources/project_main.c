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


Void clkFxn(UArg arg0) {
    if (programState == WAITING) {
        programState = READ_ACCEL_DATA;
    }
}

static void initProgram() {
    initLeds();

    //initAccelSensorTask();
    //initCommunicationTask();
    //initBuzzerTask();
    initUARTCommTask();

   Clock_Handle clkHandle;
   Clock_Params clkParams;

   Clock_Params_init(&clkParams);
   uint32_t period = 100000 / Clock_tickPeriod;
   clkParams.period = period;
   clkParams.startFlag = TRUE;

   clkHandle = Clock_create((Clock_FuncPtr)clkFxn, period, &clkParams, NULL);
   if (clkHandle == NULL) {
      System_abort("Clock create failed");
   }
}

int main(void) {

    Board_initGeneral();

    Board_initI2C();

    initProgram();

    BIOS_start();

    return (0);
}

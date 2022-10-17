/* C Standard library */
#include <stdio.h>

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
#include "buzzer.h"

#include "stateMachine.h"
#include "accelData.h"


state programState = WAITING;

/* Task */
#define STACKSIZE 2048

// Buzzer configuration
static PIN_Handle hBuzzer;
static PIN_State sBuzzer;
PIN_Config cBuzzer[] = {
  Board_BUZZER | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  PIN_TERMINATE
};


void buzzerTaskFxn(UArg arg0, UArg arg1) {

    while (1) {
      buzzerOpen(hBuzzer);
      buzzerSetFrequency(2000);
      Task_sleep(50000 / Clock_tickPeriod);
      buzzerClose();

      Task_sleep(950000 / Clock_tickPeriod);
    }
}

int main(void) {

    Board_initGeneral();

    Board_initI2C();

    initAccelSensorTask();
    /* Sanity check */
    System_printf("Hello world!\n");
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}

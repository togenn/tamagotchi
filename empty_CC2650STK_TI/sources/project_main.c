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

#include "stateMachine.h"


state programState = WAITING;

/* Task */
#define STACKSIZE 2048
Char sensorTaskStack[STACKSIZE];
Char uartTaskStack[STACKSIZE];

double ambientLight = -1000.0;

static PIN_Handle buttonHandle;
static PIN_State buttonState;
static PIN_Handle ledHandle;
static PIN_State ledState;

PIN_Config buttonConfig[] = {
   Board_BUTTON0  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
   PIN_TERMINATE };

PIN_Config ledConfig[] = {
   Board_LED0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
   PIN_TERMINATE };

void buttonFxn(PIN_Handle handle, PIN_Id pinId) {

    uint_t pinValue = PIN_getOutputValue( Board_LED0 );
    pinValue = !pinValue;
    PIN_setOutputValue( ledHandle, Board_LED0, pinValue );

}

/* Task Functions */
void uartTaskFxn(UArg arg0, UArg arg1) {

    while (1) {

        System_printf("uartTask\n");
        System_flush();

        Task_sleep(1000000 / Clock_tickPeriod);
    }
}


void sensorTaskFxn(UArg arg0, UArg arg1) {

    I2C_Handle      i2c;
    I2C_Params      i2cParams;

    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;

    i2c = I2C_open(Board_I2C_TMP, &i2cParams);
    if (i2c == NULL) {
       System_abort("Error Initializing I2C\n");
    }

   Task_sleep(100000 / Clock_tickPeriod);
   opt3001_setup(&i2c);

    while (1) {

        double data = opt3001_get_data(&i2c);
        char data_str[5];
        sprintf(data_str, "Data: %.2f\n", data);
        System_printf(data_str);

        System_printf("sensorTask\n");
        System_flush();

        Task_sleep(1000000 / Clock_tickPeriod);
    }
}

void buzzerTaskFxn(Uarg arg0, Uarg arg1) {

    while (1) {
      buzzerOpen(hBuzzer);
      buzzerSetFrequency(2000);
      Task_sleep(50000 / Clock_tickPeriod);
      buzzerClose();

      Task_sleep(950000 / Clock_tickPeriod);
    }
}

int main(void) {

    // Task variables
    Task_Handle sensorTaskHandle;
    Task_Params sensorTaskParams;
    Task_Handle uartTaskHandle;
    Task_Params uartTaskParams;

    // Initialize board
    Board_initGeneral();
    //Init6LoWPAN();
    
    Board_initI2C();

    buttonHandle = PIN_open(&buttonState, buttonConfig);
    if(!buttonHandle) {
       System_abort("Error initializing button pins\n");
    }
    ledHandle = PIN_open(&ledState, ledConfig);
    if(!ledHandle) {
       System_abort("Error initializing LED pins\n");
    }

    if (PIN_registerIntCb(buttonHandle, &buttonFxn) != 0) {
          System_abort("Error registering button callback function");
    }

    // Buzzer
    hBuzzer = PIN_open(&sBuzzer, cBuzzer);
    if (hBuzzer == NULL) {
      System_abort("Pin open failed!");
    }

    /* Task */
    Task_Params_init(&sensorTaskParams);
    sensorTaskParams.stackSize = STACKSIZE;
    sensorTaskParams.stack = &sensorTaskStack;
    sensorTaskParams.priority=2;
    sensorTaskHandle = Task_create(sensorTaskFxn, &sensorTaskParams, NULL);
    if (sensorTaskHandle == NULL) {
        System_abort("Task create failed!");
    }

    Task_Params_init(&uartTaskParams);
    uartTaskParams.stackSize = STACKSIZE;
    uartTaskParams.stack = &uartTaskStack;
    uartTaskParams.priority=2;
    uartTaskHandle = Task_create(uartTaskFxn, &uartTaskParams, NULL);
    if (uartTaskHandle == NULL) {
        System_abort("Task create failed!");
    }

    /* Sanity check */
    System_printf("Hello world!\n");
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}

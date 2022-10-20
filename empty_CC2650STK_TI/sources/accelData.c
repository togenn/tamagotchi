/*
 * accelData.c
 *
 *  Created on: 11 Oct 2022
 *      Author: Toni
 */
#include "accelData.h"
#include "stateMachine.h"

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/i2c/I2CCC26XX.h>
#include <xdc/runtime/System.h>
#include "Board.h"

#include "mpu9250.h"

#include <stdio.h>
#include "led.h"

command commandToSend = EMPTY;

#define STACKSIZE 1024
static char taskStack[STACKSIZE];

void initAccelSensorTask(void) {
    Task_Params taskParams;
    Task_Handle taskHandle;

    Task_Params_init(&taskParams);
    taskParams.stackSize = STACKSIZE;
    taskParams.stack = taskStack;
    taskParams.priority = 2;

    taskHandle = Task_create((Task_FuncPtr) accelSensorTaskFxn, &taskParams, NULL);
    if (taskHandle == NULL) {
        System_abort("Acceleration sensor task creation failed\n");
    }
}

void initAccelSensor(I2C_Handle* i2c, I2C_Params* i2cParams) {
    I2C_Params_init(i2cParams);

    static const I2CCC26XX_I2CPinCfg i2cMPUCfg = {
        .pinSDA = Board_I2C0_SDA1,
        .pinSCL = Board_I2C0_SCL1
    };
    i2cParams->custom = (uintptr_t) &i2cMPUCfg;
    i2cParams->bitRate = I2C_400kHz;

    powerOnAccelSensor();
    Task_sleep(100000 / Clock_tickPeriod);

    *i2c = I2C_open(Board_I2C, i2cParams);
    if (i2c == NULL) {
        System_abort("Error Initializing I2CMPU\n");
    }

    mpu9250_setup(i2c);

    I2C_close(*i2c);
}

void powerOnAccelSensor() {
    static PIN_Handle powerPin;
    static PIN_State  MpuPinState;
    static const PIN_Config MpuPinConfig[] = {
        Board_MPU_POWER  | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
        PIN_TERMINATE
    };
    powerPin = PIN_open(&MpuPinState, MpuPinConfig);

    PIN_setOutputValue(powerPin, Board_MPU_POWER, Board_MPU_POWER_ON);
}

void accelSensorTaskFxn(UArg arg0, UArg arg1) {

    I2C_Handle i2c;
    I2C_Params i2cParams;
    initAccelSensor(&i2c, &i2cParams);

    uint8_t index = 0;
    struct data_point data;
    struct data_point data_values[3];
    while (1) {

        if (programState == WAITING) {
            i2c = I2C_open(Board_I2C, &i2cParams);
            float gx, gy, gz;
            mpu9250_get_data(&i2c, &data.x, &data.y, &data.z, &gx, &gy, &gz);
            I2C_close(i2c);

            data_values[index] = data;
            /*
            char str[64];
            sprintf(str, "%.2f,%.2f,%.2f\n", data.x, data.y, data.z);
            System_printf(str);
            */
            if (++index > 2) {
                commandToSend = recogniseCommand(data_values);
                index = 0;
            }
        }
        Task_sleep(100000 / Clock_tickPeriod);
    }
}

command recogniseCommand(struct data_point* data) {

    return EMPTY;
}



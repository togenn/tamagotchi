/*
 * accelData.c
 *
 *  Created on: 11 Oct 2022
 *      Author: Toni, Tuukka
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


#define STACKSIZE 2048
static char taskStack[STACKSIZE];

#define DATA_POINTS 3

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
    struct data_point data_values[DATA_POINTS];
    while (1) {

        if (programState == WAITING) {
            i2c = I2C_open(Board_I2C, &i2cParams);
            mpu9250_get_data(&i2c, &data.ax, &data.ay, &data.az, &data.rx, &data.ry, &data.rz);
            I2C_close(i2c);

            data_values[index] = data;

            char str[64];
            sprintf(str, "%.2f,%.2f,%.2f\n", data.rx, data.ry, data.rz);
            System_printf(str);

            if (++index > 2) {
                System_flush();
                commandToSend = recogniseCommand(data_values);
                index = 0;
            }
        }
        Task_sleep(1000000 / Clock_tickPeriod);
    }
}

command recogniseCommand(struct data_point* data) {

    float xyzArr[3][3];
    for (int i = 0; i < 3; ++i) {
        xyzArr[0][i] = data->ax;
        xyzArr[1][i] = data->ay;
        xyzArr[2][i] = (abs(data->az)-1);
    }

    float rowsum[3];
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            rowsum[r] += xyzArr[r][c];
        }
    }

    if (abs(rowsum[0]) > 1 && rowsum[1] < 1 && rowsum[2] < 1) {
        return PET;

    } else if (abs(rowsum[1]) > 1 && rowsum[2] < 1 && rowsum[0] < 1) {
        return EXERCISE;

    } else if (rowsum[2] > 1 && rowsum[0] < 1 && rowsum[1] < 1) {
        return EAT;

    } else {
        return EMPTY_COMMAND;
    }
}



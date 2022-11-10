/*
 * accelData.c
 *
 *  Created on: 11 Oct 2022
 *      Author: Toni, Tuukka
 */
#include "accelData.h"
#include "stateMachine.h"
#include "led.h"

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

    uint8_t dataCollected = 0;
    uint8_t commandsAnalyzed = 0;
    struct data_point data;
    struct data_point data_values[DATA_POINTS];
    while (1) {

        if (programState == READ_ACCEL_DATA) {
            changeLedState(led1Handle);
            i2c = I2C_open(Board_I2C, &i2cParams);
            mpu9250_get_data(&i2c, &data.ax, &data.ay, &data.az, &data.rx, &data.ry, &data.rz);
            I2C_close(i2c);

            data_values[dataCollected] = data;
            /*
            char str[64];
            sprintf(str, "%.2f,%.2f,%.2f\n", data.rx, data.ry, data.rz);
            System_printf(str);
            */

            if (++dataCollected == DATA_POINTS) {
                //System_flush();
                recogniseCommand(data_values, commandsAnalyzed);
                dataCollected = 0;

                ++commandsAnalyzed;
            }

            if (commandsAnalyzed == COMM_INTERVAL) {
                programState = COMMUNICATION;
                commandsAnalyzed = 0;
            } else {
                programState = WAITING;
            }
        }
        Task_sleep(90000 / Clock_tickPeriod);
    }
}

void recogniseCommand(struct data_point* data, uint8_t commandsAnalyzed) {

    // Gathering 3 samples from each axis of the accelerometer
    // and normalizing the z-axis (orientation does not matter)
    float xyzAccArr[3][3];
    for (int i = 0; i < 3; ++i) {
        xyzAccArr[0][i] = data->ax;
        xyzAccArr[1][i] = data->ay;
        xyzAccArr[2][i] = (abs(data->az)-1);
    }

    // Gathering 3 samples from each axis of the gyroscope,
    // and taking the absolute values from each sample
   float xyzGyroArr[3][3];
   for (int j = 0; j < 3; ++j) {
       xyzGyroArr[0][j] = abs(data->rx);
       xyzGyroArr[1][j] = abs(data->ry);
       xyzGyroArr[2][j] = abs(data->rz);
   }

    // Summing the accelerometer values together for comparison
    float rowsum[3];
    for (int rA = 0; rA < 3; ++rA) {
        for (int cA = 0; cA < 3; ++cA) {
            rowsum[rA] += xyzAccArr[rA][cA];
        }
    }

    // Limit for gyro rotation
    float const gyroL = 30;

    // Array for each gyro axis, to check if there was rotation over the previous limit
    int rotArr[3];

    // Checking the rotations
    for (int rG = 0; rG < 3; ++rG) {
        for (int rC = 0; rC < 3; ++rC) {
            if (xyzGyroArr[rG][rC] > gyroL) {
                rotArr[rG] = 1;
                break;
            } else {
                rotArr[rG] = 1;
            }
        }
    }
    // Conditions for the commands
    command commandToSend = EMPTY_COMMAND;
    if (abs(rowsum[0]) > 1 && rowsum[1] < 1 && rowsum[2] < 1 && rotArr[1] && rotArr[2]) {
        commandToSend = PET;
    } else if (abs(rowsum[1]) > 1 && rowsum[2] < 1 && rowsum[0] < 1 && rotArr[0] && rotArr[2]) {
        commandToSend = EXERCISE;
    } else if (rowsum[2] > 1 && rowsum[0] < 1 && rowsum[1] < 1 && rotArr[0] && rotArr[1]) {
        commandToSend = EAT;
    }

    commandsToSend[commandsAnalyzed] = commandToSend;
}



/*
 * accelData.c
 *
 *  Created on: 11 Oct 2022
 *      Author: Toni, Tuukka
 */

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/i2c/I2CCC26XX.h>
#include <xdc/runtime/System.h>
#include "Board.h"
#include "mpu9250.h"
#include <stdio.h>

#include "accelData.h"
#include "stateMachine.h"
#include "tamagotchiState.h"


#define STACKSIZE 2048
static char taskStack[STACKSIZE];

#define DATA_POINTS COMM_AMOUNT + 2

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
        System_abort("Error Initializing I2C MPU\n");
    }

    mpu9250_setup(i2c);

    I2C_close(*i2c);
}

void powerOnAccelSensor() {
    PIN_Handle powerPin;
    PIN_State  MpuPinState;
    const PIN_Config MpuPinConfig[] = {
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
    struct data_point data;
    struct data_point data_values[DATA_POINTS];
    while (1) {

        if (programState == READ_ACCEL_DATA) {
            i2c = I2C_open(Board_I2C, &i2cParams);
            mpu9250_get_data(&i2c, &data.ax, &data.ay, &data.az, &data.rx, &data.ry, &data.rz);
            I2C_close(i2c);

            data_values[dataCollected] = data;
            /*
            char str[64];
            sprintf(str, "%.2f,%.2f,%.2f\n", data.ax, data.ay, data.az);
            System_printf(str);
            */


            if (++dataCollected == DATA_POINTS) {
                recogniseCommand(data_values);
                dataCollected = 0;
                programState = UPDATE_UI;
            }
        }
        Task_sleep(90000 / Clock_tickPeriod);
    }
}

void recogniseCommand(struct data_point* data) {

    // Gathering samples from each axis of the accelerometer
    // and normalizing the z-axis (orientation does not matter)
    float xyzAccArr[3][DATA_POINTS];
    for (int i = 0; i < DATA_POINTS; ++i) {
        xyzAccArr[0][i] = data[i].ax;
        xyzAccArr[1][i] = data[i].ay;
        xyzAccArr[2][i] = (fabs(data[i].az)-1);
    }

   // Gathering samples from each axis of the gyroscope,
   // and taking the absolute values from each sample
   float xyzGyroArr[3][DATA_POINTS];
   for (int j = 0; j < (DATA_POINTS + 1); ++j) {
       xyzGyroArr[0][j] = abs(data[j].rx);
       xyzGyroArr[1][j] = abs(data[j].ry);
       xyzGyroArr[2][j] = abs(data[j].rz);
   }

   // Sweeping through the 5 data points, three at a time
   for (int k = 0; k < COMM_AMOUNT; ++k) {

        // Summing the accelerometer values together for comparison
        float rowsum[3] = {0, 0, 0};
        for (int rA = 0; rA < 3; ++rA) {
            for (int cA = k; cA < (COMM_AMOUNT + k); ++cA) {
                rowsum[rA] += xyzAccArr[rA][cA];
            }
        }

        // Limit for gyro rotation
        float const gyroL = 90;

        // Array for each gyro axis, to check if there was rotation over the previous limit
        int rotArr[3];

        // Checking the rotations
        for (int rG = 0; rG < 3; ++rG) {
            for (int cG = k; cG < (COMM_AMOUNT + k); ++cG) {
                if (xyzGyroArr[rG][cG] > gyroL) {
                    rotArr[rG] = 1;
                    break;
                } else {
                    rotArr[rG] = 1;
                }
            }
        }

        // Conditions for the commands
        if (abs(rowsum[0]) > 1 && rowsum[1] < 1 && rowsum[2] < 1 && rotArr[1] && rotArr[2]) {
            commandsToSend.petAmount++;
            tState = OK;
        } else if (abs(rowsum[1]) > 1 && rowsum[2] < 1 && rowsum[0] < 1 && rotArr[0] && rotArr[2]) {
            commandsToSend.exerciseAmount++;
            tState = OK;
        } else if (rowsum[2] > 1 && rowsum[0] < 1 && rowsum[1] < 1 && rotArr[0] && rotArr[1]) {
            commandsToSend.eatAmount++;
            tState = OK;
        }

    }
}



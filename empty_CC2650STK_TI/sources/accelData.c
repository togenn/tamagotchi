/*
 * accelData.c
 *
 *  Created on: 11 Oct 2022
 *      Author: Toni
 */
#include "accelData.h"
#include "stateMachine.h"

#include <ti/drivers/I2C.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/System.h>
#include "Board.h"

#include "mpu9250.h"

command commandToSend = EMPTY;

void accelSensorTaskFxn(UArg arg0, UArg arg1) {

    I2C_Handle      i2c;
    I2C_Params      i2cParams;

    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;

    i2c = I2C_open(Board_I2C0, &i2cParams);
    if (i2c == NULL) {
       System_abort("Error Initializing I2C\n");
    }

    mpu9250_setup(&i2c);

    uint8_t index = 0;
    struct data_point data;
    struct data_point data_values[3];
    while (1) {

        if (programState == WAITING) {
            programState = READ_ACCEL_DATA;
            float gx, gy, gz;
            mpu9250_get_data(&i2c, &data.x, &data.y, &data.z, &gx, &gy, &gz);
            data_values[index] = data;

            if (++index > 2) {
                commandToSend = recogniseCommand(data_values);
                index = 0;
            }
            programState = WAITING;
            Task_sleep(100000 / Clock_tickPeriod);
        }
    }
}

command recogniseCommand(struct data_point* data) {

    return EMPTY;
}



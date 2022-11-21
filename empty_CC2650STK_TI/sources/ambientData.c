/*
 * ambientData.c
 *
 *  Created on: 18 Nov 2022
 *      Author: Toni
 */

#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC26XX.h>
#include <stdio.h>
#include "Board.h"
#include "tmp007.h"
#include "opt3001.h"

#include "ambientData.h"
#include "commands.h"

#define STACKSIZE 2048
static char taskStack[STACKSIZE];

#define TMP_HOT_LIMIT 35
#define TMP_COLD_LIMIT 20

#define BRIGHTNESS_SUNNY_LIMIT 300
#define BRIGHTNESS_DARK_LIMIT 30

void initAmbientDataTask(void) {
    Task_Params taskParams;
    Task_Handle taskHandle;

    Task_Params_init(&taskParams);
    taskParams.stackSize = STACKSIZE;
    taskParams.stack = taskStack;
    taskParams.priority = 2;

    taskHandle = Task_create((Task_FuncPtr) ambientDataTaskFxn, &taskParams, NULL);
    if (taskHandle == NULL) {
        System_abort("Communication task creation failed\n");
    }
}

void ambientDataTaskFxn(UArg arg1, UArg arg2) {
    I2C_Handle i2c;
    I2C_Params i2cParams;
    I2C_Params_init(&i2cParams);
    i2c = I2C_open(Board_I2C, &i2cParams);
    if (i2c == NULL) {
        System_abort("Error Initializing MPU\n");
    }

    tmp007_setup(&i2c);
    opt3001_setup(&i2c);
    I2C_close(i2c);

    while (1) {

        if (programState == AMBIENT_DATA) {
            i2c = I2C_open(Board_I2C, &i2cParams);

            customMsg currentMsg1 = commandsToSend.msg1ToSend;
            double tmp = tmp007_get_data(&i2c);
            if (tmp > TMP_HOT_LIMIT) {
                commandsToSend.msg1ToSend = HOT;
            } else if (tmp < TMP_COLD_LIMIT) {
                commandsToSend.msg1ToSend = COLD;
            } else {
                commandsToSend.msg1ToSend = WARM;
            }


            customMsg currentMsg2 = commandsToSend.msg2ToSend;
            double brightness = opt3001_get_data(&i2c);
            if (brightness == -1.0) {
                //data was not ready
            } else if (brightness > BRIGHTNESS_SUNNY_LIMIT) {
                commandsToSend.msg2ToSend = SUNNY;
            } else if (brightness < BRIGHTNESS_DARK_LIMIT) {
                commandsToSend.msg2ToSend = DARK;
            } else {
                commandsToSend.msg2ToSend = GOOD_LIGHT;
            }

            if (currentMsg1 != commandsToSend.msg1ToSend || currentMsg2 != commandsToSend.msg2ToSend) {
                commandsToSend.customMsgSent = false;
            }

            I2C_close(i2c);

            programState = COMMUNICATION;
        }


        Task_sleep(900000 / Clock_tickPeriod);
    }
}

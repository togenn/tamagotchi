/*
 * ambientData.c
 *
 *  Created on: 18 Nov 2022
 *      Author: Toni
 */
#include "ambientData.h"

#define STACKSIZE 1024

void initAmbientDataTask(void) {
    Task_Params taskParams;
    Task_Handle taskHandle;

    Task_Params_init(&taskParams);
    taskParams.stackSize = STACKSIZE;
    taskParams.stack = taskStack;
    taskParams.priority = 2;

    taskHandle = Task_create((Task_FuncPtr) communicationTaskFxn, &taskParams, NULL);
    if (taskHandle == NULL) {
        System_abort("Communication task creation failed\n");
    }
}


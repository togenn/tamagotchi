/*
 * update_ui.c
 *
 *  Created on: 14 Nov 2022
 *      Author: Samuli
 */
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/System.h>
#include <ti/drivers/PIN.h>
#include "update_ui.h"
#include "notePlayer.h"
#include "led.h"
#include "tamagotchiState.h"
#include "notePlayer.h"
#include "notePitches.h"
#include "commands.h"
#include "buzzer.h"
#include "musicPlayer.h"



// green led = led1Handle, led1State
#define greenLedHandle led1Handle
// red led = led2Handle, led2State
#define redLedHandle led2Handle

#define STACKSIZE 1024
static char taskStack[STACKSIZE];
PIN_Handle hBuzzer;


void initUpdateUITask(void) {

    Task_Handle task;
    Task_Params taskParams;

    initLeds();

    initBuzzer();
    hBuzzer = getBuzzerHandle();
    if (hBuzzer == NULL) {
        System_abort("Pin open failed! ui");
    }

    Task_Params_init(&taskParams);
    taskParams.stackSize = STACKSIZE;
    taskParams.stack = &taskStack;
    taskParams.priority = 2;
    task = Task_create((Task_FuncPtr)updateUIFxn, &taskParams, NULL);
    if (task == NULL) {
       System_abort("Update UI Task creation failed!");
    }

    initBackgroundMusic();


}


void updateUIFxn(UArg arg0, UArg arg1) {
    while (1) {
        if (programState == UPDATE_UI) {
            bool commandRecognized = checkForCommand();
            doBuzzerTask(commandRecognized);
            doLedTask(commandRecognized);
            programState = AMBIENT_DATA;
        }
        Task_sleep(100000 / Clock_tickPeriod);
    }
}


void doBuzzerTask(bool commandRecognized) {
    if (commandRecognized) {
        stopMusic(); // Interrupt bg music
        closeBuzzer();
        // Play cmd jingle
        noteInfo currentMelody[] = {{NOTE_E3, 400}, {NOTE_E4, 400}};
        size_t melodySize = sizeof(currentMelody) / sizeof(currentMelody[0]);
        openBuzzer(hBuzzer);
        playMelody(currentMelody, melodySize);
        closeBuzzer();
        startMusic();

    }

    static bool musicStopped = false;
    if (tState == CRITICAL) {
        stopMusic(); // Interrupt bg music
        musicStopped = true;
        closeBuzzer();
        noteInfo currentMelody[] = {{NOTE_G3, 400}, {NOTE_C4, 400}};
        size_t melodySize = sizeof(currentMelody) / sizeof(currentMelody[0]);
        openBuzzer(hBuzzer);
        playMelody(currentMelody, melodySize);
        closeBuzzer();
    } else if (tState == OK && musicStopped) {
        startMusic();
        musicStopped = false;
    }
}


void doLedTask(bool commandRecognized) {
    uint8_t redLedStatus = getLedState(redLedHandle); // 0 = off, 1 = on
    uint8_t greenLedStatus = getLedState(greenLedHandle);
    if (commandRecognized) {
        changeLedState(greenLedHandle);
        DELAY_MS(500);
        changeLedState(greenLedHandle);
    }
    if (tState == CRITICAL && redLedStatus == 0) {
        changeLedState(redLedHandle);
    } else if (tState == OK && redLedStatus == 1) {
        changeLedState(redLedHandle);
    }
}

bool checkForCommand() {
    if (commandsToSend.eatAmount > 0 || commandsToSend.petAmount > 0 || commandsToSend.exerciseAmount > 0) {
        return true;
    }
    return false;
}






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

//note lenghts in ms
#define BPM 110.0
#define QUARTER_NOTE 1 / (BPM / 60) * 1000
#define EIGHT_NOTE QUARTER_NOTE / 2
#define SIXTEENTH_NOTE EIGHT_NOTE / 2

// Buzzer configuration
static PIN_Handle hBuzzer;
static PIN_State sBuzzer;
PIN_Config cBuzzer[] = {
  Board_BUZZER | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  PIN_TERMINATE
};
// Background music
//notes for the song: https://github.com/robsoncouto/arduino-songs/blob/master/doom/doom.ino
noteInfo bgMusic[] = {{NOTE_E2, 800}, {NOTE_E2, 800}, {NOTE_E3, 800}, {NOTE_E2, 800},
                      {NOTE_E2, 800}, {NOTE_D3, 800}, {NOTE_E2, 800}, {NOTE_E2, 800},
                      {NOTE_C3, 800}, {NOTE_E2, 800}, {NOTE_E2, 800}, {NOTE_AS2, 800},
                      {NOTE_E2, 800}, {NOTE_E2, 800}, {NOTE_B2, 800}, {NOTE_C3, 800},
                      {NOTE_E2, 800}, {NOTE_E2, 800}, {NOTE_E3, 800}, {NOTE_E2, 800},
                      {NOTE_E2, 800}, {NOTE_D3, 800}, {NOTE_E2, 800}, {NOTE_E2, 800},
                      {NOTE_C3, 800}, {NOTE_E2, 800}, {NOTE_E2, 800}, {NOTE_AS2, 800},
                      {NOTE_AS2, 800}, {NOTE_AS2, 800}, {NOTE_AS2, 800}, {NOTE_NULL, 800},
                      {NOTE_E2, 800}, {NOTE_E2, 800}, {NOTE_E3, 800}, {NOTE_E2, 800},
                      {NOTE_E2, 800}, {NOTE_D3, 800}, {NOTE_E2, 800}, {NOTE_E2, 800},
                      {NOTE_C3, 800}, {NOTE_E2, 800}, {NOTE_E2, 800}, {NOTE_AS2, 800},
                      {NOTE_E2, 800}, {NOTE_E2, 800}, {NOTE_B2, 800}, {NOTE_C3, 800},
                      {NOTE_E2, 800}, {NOTE_E2, 800}, {NOTE_E3, 800}, {NOTE_E2, 800},
                      {NOTE_E2, 800}, {NOTE_D3, 800}, {NOTE_E2, 800}, {NOTE_E2, 800},
                      {NOTE_C3, 800}, {NOTE_E2, 800}, {NOTE_E2, 800}, {NOTE_AS2, 800},
                      {NOTE_AS2, 800}, {NOTE_AS2, 800}, {NOTE_AS2, 800}, {NOTE_NULL, 800},
                      {NOTE_A2, 8}, {NOTE_A2, 8}, {NOTE_A3, 8}, {NOTE_A2, 8},
                      {NOTE_A2, 8}, {NOTE_G3, 8}, {NOTE_A2, 8}, {NOTE_A2, 8},
                      {NOTE_F3, 8}, {NOTE_A2, 8}, {NOTE_A2, 8}, {NOTE_DS3, 8},
                      {NOTE_A2, 8}, {NOTE_A2, 8}, {NOTE_E3, 8}, {NOTE_F3, 8},
                      {NOTE_A2, 8}, {NOTE_A2, 8}, {NOTE_A3, 8}, {NOTE_A2, 8},
                      {NOTE_A2, 8}, {NOTE_G3, 8}, {NOTE_A2, 8}, {NOTE_A2, 8},
                      {NOTE_F3, 8}, {NOTE_A2, 8}, {NOTE_A2, 8}, {NOTE_DS3, 8},
                      {NOTE_DS3, 8}, {NOTE_DS3, 8}, {NOTE_DS3, 8},{NOTE_NULL, 800},
                      {NOTE_A2, 8}, {NOTE_A2, 8}, {NOTE_A3, 8}, {NOTE_A2, 8},
                      {NOTE_A2, 8}, {NOTE_G3, 8}, {NOTE_A2, 8}, {NOTE_A2, 8},
                      {NOTE_F3, 8}, {NOTE_A2, 8}, {NOTE_A2, 8}, {NOTE_DS3, 8},
                      {NOTE_A2, 8}, {NOTE_A2, 8}, {NOTE_E3, 8}, {NOTE_F3, 8},
                      {NOTE_A2, 8}, {NOTE_A2, 8}, {NOTE_A3, 8}, {NOTE_A2, 8},
                      {NOTE_A2, 8}, {NOTE_G3, 8}, {NOTE_A2, 8}, {NOTE_A2, 8},
                      {NOTE_F3, 8}, {NOTE_A2, 8}, {NOTE_A2, 8}, {NOTE_DS3, 8},
                      {NOTE_DS3, 8}, {NOTE_DS3, 8}, {NOTE_DS3, 8},{NOTE_NULL, 800}
};

// green led = led1Handle, led1State
#define greenLedHandle led1Handle
// red led = led2Handle, led2State
#define redLedHandle led2Handle

#define STACKSIZE 1024
static char taskStack[STACKSIZE];



void initUpdateUITask(void) {
    // Init LEDs
    initLeds();

    Task_Handle task;
    Task_Params taskParams;
    // Buzzer
    hBuzzer = PIN_open(&sBuzzer, cBuzzer);
    if (hBuzzer == NULL) {
        System_abort("Pin open failed!");
    }

    Task_Params_init(&taskParams);
    taskParams.stackSize = STACKSIZE;
    taskParams.stack = &taskStack;
    taskParams.priority = 2;
    task = Task_create((Task_FuncPtr)updateUIFxn, &taskParams, NULL);
    if (task == NULL) {
       System_abort("Update UI Task creation failed!");
    }

    Clock_Handle clkHandle;
    Clock_Params clkParams;

    Clock_Params_init(&clkParams);
    uint32_t period = 136000 / Clock_tickPeriod;
    clkParams.period = period;
    clkParams.startFlag = TRUE;

    clkHandle = Clock_create((Clock_FuncPtr) musicTimerFxn, period, &clkParams, NULL);
    if (clkHandle == NULL) {
       System_abort("Clock create failed");
    }


}

void musicTimerFxn(UArg arg0) {
    static int noteCounter = 0;
    static size_t melodySize = sizeof(bgMusic) / sizeof(bgMusic[0]);
    openBuzzer(hBuzzer);
    uint16_t note = bgMusic[noteCounter].note;
    buzzerSetFrequency(note);
    noteCounter++;
    if (noteCounter == melodySize) {
        closeBuzzer();
        noteCounter = 0;
    }
}


void updateUIFxn(UArg arg0, UArg arg1) {
    while (1) {
        if (programState == UPDATE_UI) {
            bool commandRecognized = checkForCommand();
            doBuzzerTask(commandRecognized);
            doLedTask(commandRecognized);
            programState = COMMUNICATION;
        }
        Task_sleep(100000 / Clock_tickPeriod);
    }
}


void doBuzzerTask(bool commandRecognized) {
    if (commandRecognized) {
        closeBuzzer(); // Interrupt bg music
        // Play cmd jingle
        noteInfo currentMelody[] = {{NOTE_E3, 400}, {NOTE_E4, 400}};
        size_t melodySize = sizeof(currentMelody) / sizeof(currentMelody[0]);
        openBuzzer(hBuzzer);
        playMelody(currentMelody, melodySize);
        closeBuzzer();

    }
    if (tState == CRITICAL) {
        closeBuzzer(); // Interrupt bg music
        noteInfo currentMelody[] = {{NOTE_G3, 400}, {NOTE_C4, 400}};
        size_t melodySize = sizeof(currentMelody) / sizeof(currentMelody[0]);
        openBuzzer(hBuzzer);
        playMelody(currentMelody, melodySize);
        closeBuzzer();
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






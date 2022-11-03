/*
 * notePlayer.c
 *
 *  Created on: 19 Oct 2022
 *      Author: Samuli
 */

#include "buzzer.h"
#include "notePlayer.h"
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/System.h>
#include "notePitches.h"
#include "tamagotchiState.h"
#define DELAY_MS(i)      (Task_sleep(((i) * 1000) / Clock_tickPeriod))

// Buzzer configuration
static PIN_Handle hBuzzer;
static PIN_State sBuzzer;

#define STACKSIZE 1024
static char taskStack[STACKSIZE];



PIN_Config cBuzzer[] = {
  Board_BUZZER | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  PIN_TERMINATE
};

void openBuzzer() {
    buzzerOpen(hBuzzer);
}
void closeBuzzer() {
    buzzerClose();
}

void playNote(uint16_t frequency, uint32_t duration) {
    buzzerSetFrequency(frequency);
    DELAY_MS(duration);
}

void playMelody(noteInfo* melody, size_t melodyLength) {
    for(size_t i = 0; i < melodyLength; i++) {
        playNote(melody[i].note, melody[i].duration);
    }
}

void buzzerTaskFxn(UArg arg0, UArg arg1) {
    while (1) {
        if (tState == CRITICAL) {
            noteInfo currentMelody = {{NOTE_G3, 400}, {NOTE_C4, 400}};
            size_t melodySize = sizeof(currentMelody) / sizeof(currentMelody[0]);
            openBuzzer();
            playMelody(currentMelody, melodySize);
            closeBuzzer();
        }
      Task_sleep(100000 / Clock_tickPeriod);
    }
}

void initBuzzerTask() {
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
     task = Task_create((Task_FuncPtr)buzzerTaskFxn, &taskParams, NULL);
     if (task == NULL) {
       System_abort("Buzzer task creation failed!");
     }
}

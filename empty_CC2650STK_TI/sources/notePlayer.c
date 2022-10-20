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
#define DELAY_MS(i)      (Task_sleep(((i) * 1000) / Clock_tickPeriod))

// Buzzer configuration
static PIN_Handle hBuzzer;
static PIN_State sBuzzer;
/* Task */
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

void playNote(uint16_t frequency, int duration) {
    buzzerSetFrequency(frequency);
    DELAY_MS(duration);
}

void playMelody(noteInfo melody[], int melodyLength) {
    int i;
    for(i = 0; i < melodyLength; i++) {
        playNote(melody[i].note, melody[i].duration);
    }
}

void buzzerTaskFxn(UArg arg0, UArg arg1) {
    noteInfo testMelody[] = {{NOTE_C4, 800}, {NOTE_G4, 800}};
    int melodySize = sizeof(testMelody) / sizeof(testMelody[0]);
    while (1) {
      openBuzzer();
      playMelody(testMelody, melodySize);
      closeBuzzer();

      Task_sleep(950000 / Clock_tickPeriod);
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
       System_abort("Task create failed!");
     }
}

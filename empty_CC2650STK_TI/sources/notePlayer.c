/*
 * notePlayer.c
 *
 *  Created on: 19 Oct 2022
 *      Author: Samuli
 */

#include "buzzer.h"
#include "notePlayer.h"
#include "notePitches.h"
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

#define DELAY_MS(i)      (Task_sleep(((i) * 1000) / Clock_tickPeriod))

// Buzzer configuration
PIN_Handle hBuzzer;
PIN_State sBuzzer;
PIN_Config cBuzzer[] = {
  Board_BUZZER | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  PIN_TERMINATE
};

void initBuzzerHandle() {
    hBuzzer = PIN_open(&sBuzzer, cBuzzer);
}

PIN_Handle getBuzzerHandle() {
    return hBuzzer;
}


void openBuzzer(PIN_Handle hBuzzer) {
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


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


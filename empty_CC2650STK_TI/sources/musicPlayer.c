/*
 * musicPlayer.c
 *
 *  Created on: 21 Nov 2022
 *      Author: Same
 */

#include <ti/sysbios/knl/Clock.h>

#include <xdc/runtime/System.h>
#include <ti/drivers/PIN.h>
#include "notePitches.h"
#include "notePlayer.h"
#include "buzzer.h"
#include "musicPlayer.h"




//note lenghts in ms
#define BPM 110.0
#define QUARTER_NOTE 1.0 / (BPM / 60.0) * 1000
#define EIGHT_NOTE QUARTER_NOTE / 2
#define SIXTEENTH_NOTE EIGHT_NOTE / 2

// Background music
//notes for the song: https://github.com/robsoncouto/arduino-songs/blob/master/doom/doom.ino
const static noteInfo bgMusic[] = {{NOTE_E2, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_E3, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE},
                      {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_D3, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE},
                      {NOTE_C3, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_AS2, SIXTEENTH_NOTE},
                      {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_B2, SIXTEENTH_NOTE}, {NOTE_C3, SIXTEENTH_NOTE},
                      {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_E3, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE},
                      {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_D3, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE},
                      {NOTE_C3, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_AS2, QUARTER_NOTE},
                      {NOTE_NULL, SIXTEENTH_NOTE},
                      {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_E3, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE},
                      {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_D3, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE},
                      {NOTE_C3, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_AS2, SIXTEENTH_NOTE},
                      {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_B2, SIXTEENTH_NOTE}, {NOTE_C3, SIXTEENTH_NOTE},
                      {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_E3, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE},
                      {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_D3, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE},
                      {NOTE_C3, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_E2, SIXTEENTH_NOTE}, {NOTE_AS2, QUARTER_NOTE},
                      {NOTE_NULL, SIXTEENTH_NOTE},
                      {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_A3, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE},
                      {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_G3, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE},
                      {NOTE_F3, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_DS3, SIXTEENTH_NOTE},
                      {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_E3, SIXTEENTH_NOTE}, {NOTE_F3, SIXTEENTH_NOTE},
                      {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_A3, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE},
                      {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_G3, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE},
                      {NOTE_F3, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_DS3, QUARTER_NOTE},
                      {NOTE_NULL, SIXTEENTH_NOTE},
                      {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_A3, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE},
                      {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_G3, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE},
                      {NOTE_F3, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_DS3, SIXTEENTH_NOTE},
                      {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_E3, SIXTEENTH_NOTE}, {NOTE_F3, SIXTEENTH_NOTE},
                      {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_A3, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE},
                      {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_G3, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE},
                      {NOTE_F3, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_A2, SIXTEENTH_NOTE}, {NOTE_DS3, QUARTER_NOTE},
                      {NOTE_NULL, SIXTEENTH_NOTE},
};
static Clock_Handle clkHandle;
PIN_Handle hBuzzer;
void initBackgroundMusic() {


    Clock_Params clkParams;

    Clock_Params_init(&clkParams);
    uint32_t period = 10000 / Clock_tickPeriod;
    clkParams.period = period;
    clkParams.startFlag = TRUE;

    clkHandle = Clock_create((Clock_FuncPtr) musicTimerFxn, period, &clkParams, NULL);
    if (clkHandle == NULL) {
       System_abort("Clock create failed");
    }
}


void musicTimerFxn(UArg arg0) {
    hBuzzer = getBuzzerHandle();
    if (hBuzzer == NULL) {
        System_abort("Pin open failed! music");
    }
    static int noteCounter = 0;
    static size_t melodySize = sizeof(bgMusic) / sizeof(bgMusic[0]);
    openBuzzer(hBuzzer);
    uint16_t note = bgMusic[noteCounter].note;
    uint32_t noteLength = bgMusic[noteCounter].duration;

    stopMusic();
    Clock_setTimeout(clkHandle, (noteLength * 1000) / Clock_tickPeriod);// Set clock for note length
    startMusic();

    buzzerSetFrequency(note);
    noteCounter++;
    if (noteCounter == melodySize) {
        closeBuzzer();
        noteCounter = 0;
    }
}

void startMusic() {
    Clock_start(clkHandle);
}

void stopMusic() {
    Clock_stop(clkHandle);
}


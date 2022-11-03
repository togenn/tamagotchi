/*
 * notePlayer.h
 *
 *  Created on: 19 Oct 2022
 *      Author: Samuli
 */

#ifndef HEADERS_NOTEPLAYER_H_
#define HEADERS_NOTEPLAYER_H_
#include "notePitches.h"

typedef struct {
    uint16_t note;
    uint32_t duration;
} noteInfo;

noteInfo alarmMelody[] = {{NOTE_G3, 800}, {NOTE_C4, 800}};


void initBuzzerTask();

void playNote(uint16_t frequency, uint32_t duration);
void playMelody(noteInfo* melody, size_t melodyLength);
void initBuzzer();
void closeBuzzer();
void buzzerTaskFxn(UArg arg0, UArg arg1);
#endif /* HEADERS_NOTEPLAYER_H_ */

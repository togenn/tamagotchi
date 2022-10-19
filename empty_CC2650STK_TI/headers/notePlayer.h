/*
 * notePlayer.h
 *
 *  Created on: 19 Oct 2022
 *      Author: Samuli
 */

#ifndef HEADERS_NOTEPLAYER_H_
#define HEADERS_NOTEPLAYER_H_

typedef struct {
    unsigned int note;
    unsigned long duration;
} noteInfo;

void playNote(unsigned int frequency, unsigned long duration);
void playMelody(noteInfo melody[]);
void initBuzzer();
void closeBuzzer();
void buzzerTaskFxn(UArg arg0, UArg arg1);
void initBuzzerTask();
#endif /* HEADERS_NOTEPLAYER_H_ */

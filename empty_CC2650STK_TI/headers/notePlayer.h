/*
 * notePlayer.h
 *
 *  Created on: 19 Oct 2022
 *      Author: Samuli
 */

#ifndef HEADERS_NOTEPLAYER_H_
#define HEADERS_NOTEPLAYER_H_

typedef struct {
    uint16_t note;
    uint32_t duration;
} noteInfo;

PIN_Handle getBuzzerHandle();
void initBuzzer();
void playMelody(noteInfo* melody, size_t melodyLength);
void closeBuzzer();
void openBuzzer(PIN_Handle);

//private
void playNote(uint16_t frequency, uint32_t duration);

#endif /* HEADERS_NOTEPLAYER_H_ */

/*
 * update_ui.h
 *
 *  Created on: 14 Nov 2022
 *      Author: Samuli
 */

#ifndef HEADERS_UPDATE_UI_H_
#define HEADERS_UPDATE_UI_H_



void initUpdateUITask(void);
void doBuzzerTask(bool commandRecognized);
void doLedTask(bool commandRecognized);
void updateUIFxn(UArg arg0, UArg arg1);
bool checkForCommand();
#define DELAY_MS(i)      (Task_sleep(((i) * 1000) / Clock_tickPeriod))


#endif /* HEADERS_UPDATE_UI_H_ */

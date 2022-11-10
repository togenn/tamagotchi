/*
 * led.h
 *
 *  Created on: 20 Oct 2022
 *      Author: peral
 */

#ifndef HEADERS_LED_H_
#define HEADERS_LED_H_

#include <ti/drivers/pin/PINCC26xx.h>
#include <ti/drivers/PIN.h>

extern PIN_Handle led1Handle;
extern PIN_State led1State;

extern PIN_Handle led2Handle;
extern PIN_State led2State;

void initLeds(void);
void changeLedState(PIN_Handle ledHandle);


#endif /* HEADERS_LED_H_ */

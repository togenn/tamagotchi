/*
 * led.c
 *
 *  Created on: 20 Oct 2022
 *      Author: peral
 */
#include "led.h"
#include "Board.h"

#define LED1_PIN Board_LED0
#define LED2_PIN Board_LED1

PIN_Handle led1Handle;
PIN_State led1State;

PIN_Handle led2Handle;
PIN_State led2State;

void initLeds() {
    const PIN_Config led1Config[] = {
           LED1_PIN | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
           PIN_TERMINATE};
    const PIN_Config led2Config[] = {
               LED2_PIN | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
               PIN_TERMINATE};
    led1Handle = PIN_open(&led1State, led1Config);
    PIN_setOutputValue(led1Handle, LED1_PIN, 0);

    led2Handle = PIN_open(&led2State, led2Config);
    PIN_setOutputValue(led2Handle, LED2_PIN, 0);

}

void changeLedState(PIN_Handle ledHandle) {
    uint32_t ledPin = ledHandle == led1Handle ? LED1_PIN : LED2_PIN;
    uint_t pinValue = PIN_getOutputValue(ledPin);
    pinValue = !pinValue;
    PIN_setOutputValue(ledHandle, ledPin, pinValue);
}

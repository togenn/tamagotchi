/*
 * led.c
 *
 *  Created on: 20 Oct 2022
 *      Author: peral
 */
#include "led.h"

#include <ti/drivers/pin/PINCC26xx.h>
#include <ti/drivers/PIN.h>
#include "Board.h"

static PIN_Handle ledHandle;
static PIN_State ledState;

void initLed() {
    static const PIN_Config ledConfig[] = {
           Board_LED0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
           PIN_TERMINATE};
    ledHandle = PIN_open(&ledState, ledConfig);
    PIN_setOutputValue(ledHandle, Board_LED0, 0);

}

void changeLedState() {
    uint_t pinValue = PIN_getOutputValue( Board_LED0 );
    pinValue = !pinValue;
    PIN_setOutputValue( ledHandle, Board_LED0, pinValue );
}

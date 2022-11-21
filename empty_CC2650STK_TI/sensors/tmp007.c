/*
 * tmp007.c
 *
 *  Created on: 28.9.2016
 *  Author: Teemu Leppanen / UBIComp / University of Oulu
 *
 *  Datakirja: http://www.ti.com/lit/ds/symlink/tmp007.pdf
 */

#include <xdc/runtime/System.h>
#include <string.h>
#include "Board.h"
#include "tmp007.h"

void tmp007_setup(I2C_Handle *i2c) {

	System_printf("TMP007: Config OK!\n");
    System_flush();
}

/**************** JTKJ: DO NOT MODIFY ANYTHING ABOVE THIS LINE ****************/

double tmp007_get_data(I2C_Handle *i2c) {

	double temperature = 0.0;
    char txBuffer[1] = {TMP007_REG_TEMP};
    char rxBuffer[2];
    I2C_Transaction i2cMessage;
    i2cMessage.readBuf = rxBuffer;
    i2cMessage.readCount = 2;
    i2cMessage.writeBuf = txBuffer;
    i2cMessage.writeCount = 1;
    i2cMessage.slaveAddress = Board_TMP007_ADDR;

	if (I2C_transfer(*i2c, &i2cMessage)) {

        int16_t regValue = (rxBuffer[0] << 8) | rxBuffer[1];
        regValue >>= 2;
        temperature = regValue * 0.03125;

	} else {

		System_printf("TMP007: Data read failed!\n");
		System_flush();
	}

	return temperature;
}

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

	double temperature = 0.0; // return value of the function
    // JTKJ: Find out the correct buffer sizes with this sensor?
    // char txBuffer[ n ];
    // char rxBuffer{ n ];

    // JTKJ: Fill in the i2cMessage data structure with correct values
    //       as shown in the lecture material
    I2C_Transaction i2cMessage;

	if (I2C_transfer(*i2c, &i2cMessage)) {

        // JTKJ: Here the conversion from register value to temperature

	} else {

		System_printf("TMP007: Data read failed!\n");
		System_flush();
	}

	return temperature;
}

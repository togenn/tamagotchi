/*
 * hdc1000.h
 *
 *  Created on: 22.7.2016
 *  Author: Teemu Leppanen / UBIComp / University of Oulu
 *
 * 	Datasheet: http://www.ti.com/lit/ds/symlink/hdc1000.pdf
 */

#ifndef HDC1000_H_
#define HDC1000_H_

#include <ti/drivers/I2C.h>

#define HDC1000_REG_TEMP		0x0
#define HDC1000_REG_HUM			0x1
#define HDC1000_REG_CONFIG		0x2

void hdc1000_setup(I2C_Handle *i2c);
void hdc1000_get_data(I2C_Handle *i2c, double *temp, double *hum);

#endif /* HDC1000_H_ */

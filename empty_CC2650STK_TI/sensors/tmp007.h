/*
 * tmp007.h
 *
 *  Created on: 28.9.2016
 *  Author: Teemu Leppanen / UBIComp / University of Oulu
 *
 *  Datakirja: http://www.ti.com/lit/ds/symlink/tmp007.pdf
 */

#ifndef TMP007_H_
#define TMP007_H_

#include <ti/drivers/I2C.h>

#define TMP007_REG_TEMP	0x03

void tmp007_setup(I2C_Handle *i2c);
double tmp007_get_data(I2C_Handle *i2c);

#endif /* TMP007_H_ */

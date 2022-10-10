/*
 * bmp280.h
 *
 *  Created on: 7.10.2016
 *  Author: Teemu Leppänen / UBIComp / University of Oulu
 *
 * 	Datasheet: https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMP280-DS001-12.pdf
 */

#ifndef BMP280_H_
#define BMP280_H_

#include <ti/drivers/I2C.h>

#define BMP280_REG_CTRL_MEAS	0xF4
#define BMP280_REG_CONFIG		0xF5
#define BMP280_REG_PRES_MSB		0xF7
#define BMP280_REG_T1 			0x88
/*
#define BMP280_REG_T2			0x8A
#define BMP280_REG_T3			0x8C
#define BMP280_REG_P1			0x8E
#define BMP280_REG_P2			0x90
#define BMP280_REG_P3			0x92
#define BMP280_REG_P4			0x94
#define BMP280_REG_P5			0x96
#define BMP280_REG_P6			0x98
#define BMP280_REG_P7			0x9A
#define BMP280_REG_P8			0x9C
#define BMP280_REG_P9			0x9E
*/

void bmp280_setup(I2C_Handle *i2c);
void bmp280_get_data(I2C_Handle *i2c, double *pressure, double *temperature);

#endif /* BMP280_H_ */

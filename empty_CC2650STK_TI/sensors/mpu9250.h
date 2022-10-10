/*
 * mpu9250.h
 *
 *  Adopted for SensorTag from https://github.com/kriswiner/MPU-9250 by Kris Winer
 *
 *  Created on: 8.10.2016
 *  Author: Teemu Leppanen / UBIComp / University of Oulu
 *
 * 	Datasheet: https://store.invensense.com/datasheets/invensense/MPU9250REV1.0.pdf
 */

#ifndef MPU9250_H_
#define MPU9250_H_

#include <ti/drivers/I2C.h>

void mpu9250_setup(I2C_Handle *i2c);
void mpu9250_get_data(I2C_Handle *i2c, float *ax, float *ay, float *az, float *gx, float *gy, float *gz);

#endif /* MPU9250_H_ */

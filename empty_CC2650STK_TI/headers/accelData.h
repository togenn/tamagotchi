
#ifndef HEADERS_ACCELDATA_H_
#define HEADERS_ACCELDATA_H_

#include <xdc/std.h>
#include <ti/drivers/I2C.h>

#include "commands.h"

struct data_point {
    float x;
    float y;
    float z;
};

void initAccelSensorTask(void);

void accelSensorTaskFxn(UArg arg0, UArg arg1);
command recogniseCommand(struct data_point* data);
void initAccelSensor(I2C_Handle* i2c, I2C_Params* i2cParams);
void powerOnAccelSensor(void);



#endif /* HEADERS_ACCELDATA_H_ */

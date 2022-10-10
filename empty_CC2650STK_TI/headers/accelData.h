
#ifndef HEADERS_ACCELDATA_H_
#define HEADERS_ACCELDATA_H_

#include "commands.h"

struct data_point {
    float x;
    float y;
    float z;
};

data_point data_values[3];

void accelSensorTaskFxn(UArg arg0, UArg arg1);

command recogniseCommand();



#endif /* HEADERS_ACCELDATA_H_ */


#ifndef HEADERS_ACCELDATA_H_
#define HEADERS_ACCELDATA_H_

#include <xdc/std.h>

#include "commands.h"

struct data_point {
    float x;
    float y;
    float z;
};


void accelSensorTaskFxn(UArg arg0, UArg arg1);

command recogniseCommand(struct data_point* data);



#endif /* HEADERS_ACCELDATA_H_ */

#ifndef __HYBRID_LASER_H
#define __HYBRID_LASER_H

#include "laser_sensor.h"
#include "robocon.h"
#include "approx_math.h"
#include "stdbool.h"
#include "pk_math.h"

extern int increment;
extern int haha;
extern int laserB;
extern int laserW;
//extern u16 ls_cal_reading[2];

#define RED_SIDE 0
#define BLUE_SIDE 1

void enterPole(void);
void laserPID(void);

#endif

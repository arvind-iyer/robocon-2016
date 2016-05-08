#ifndef __HYBRID_LASER_H
#define __HYBRID_LASER_H

#include "laser_sensor.h"
#include "stdbool.h"

extern u16 ls_cal_reading[2];
extern bool robotMode;

#define RED_SIDE 0
#define BLUE_SIDE 1

void enterPole();

#endif
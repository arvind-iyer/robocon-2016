#ifndef IMU_MATH_H
#define IMU_MATH_H

#include "stm32f4xx.h"
#include <stdbool.h>

float abs_diff(float minuend, float subtrahend);
float range_remap(float to_be_mapped);

#endif

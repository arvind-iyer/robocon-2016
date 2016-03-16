#ifndef IMU_MATH_H
#define IMU_MATH_H

#include "stm32f4xx.h"
#include <stdbool.h>
#include "mpu6050.h"
#include "math.h"
#include "approx_math.h"
#include <cstring>
#include "main.h"

#define GYRO_GAIN 32768/2000 

extern s32 ypr[3];

void calcIMU();

#endif

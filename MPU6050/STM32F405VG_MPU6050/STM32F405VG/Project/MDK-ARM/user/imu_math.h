#ifndef IMU_MATH_H
#define IMU_MATH_H

#include "stm32f4xx.h"
#include <stdbool.h>
#include "mpu6050.h"
#include "math.h"
#include "approx_math.h"
#include <cstring>
#include "main.h"

#define pie 3.14159265358979f
#define GYRO_TRUST 0

extern float ypr[3];
extern float Rest[3];

void normalise_vector_float(float* v);
float sqr(float num);

void calc_init(void);
void calcIMU(void);

#endif

#ifndef IMU_MATH_H
#define IMU_MATH_H

#include "stm32f4xx.h"
#include <stdbool.h>
#include "mpu6050.h"
#include "approx_math.h"
#include <cmath>
#include <cstring>
#include "main.h"
#include "vector_math.h"
#include "matrix_math.h"

#define pi 3.14159265358979f
#define GYRO_TRUST 10

#define IMU_ACCEL_P 30.0f
#define IMU_ACCEL_I 0.00008f

extern float ypr[3];
extern f_matrix DCM_B;

void normalise_vector_float(float* v);
s16 sqr(s16 num);

void calc_init(void);
void calcIMU(void);

#endif

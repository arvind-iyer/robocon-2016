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

#define IMU_ACCEL_P 15000.0f
#define IMU_ACCEL_I 0.00008f

#define ACCEL_1G_POS_X 8370
#define ACCEL_1G_NEG_X 7856
#define ACCEL_1G_POS_Y 8112
#define ACCEL_1G_NEG_Y 8274
#define ACCEL_1G_POS_Z 6868
#define ACCEL_1G_NEG_Z 9878

#define GYRO_X_STATIC_OFFSET 25.64f
#define GYRO_Y_STATIC_OFFSET 55.6f
#define GYRO_Z_STATIC_OFFSET 2.5f

#define YAW_USING_ABS_GYRO true

extern float ypr[3];
extern f_matrix DCM_B;
extern bool imu_ignoring_accel, imu_gyro_saturated;

void normalise_vector_float(float* v);
s16 sqr(s16 num);

void calc_init(void);
void calcIMU(void);
void calibration_mode_loop(void);

#endif

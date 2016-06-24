#ifndef VELOCITY_H
#define VELOCITY_H

#include "stm32f4xx.h"
#include "quick_math.h"
#include "encoder.h"
#include "MTi_adp.h"
#include "MTi-1_UART.h"
#include "vector_math.h"

#define VEL_ACCEL_TRUST 600 //Scaled by 1000

void vel_update(void);
s32 get_vel(void);

#endif

#ifndef __RPM_SENSOR_H__
#define	__RPM_SENSOR_H__

#include "stm32f10x_tim.h"
#include "encoder.h"
#include "servo.h"
#include "approx_math.h"

#define RPMs_Count_TIM	TIM6
#define RPMs_encoder		ENCODER2

#define	countPerRo			6
#define overFlow				60000

void RPMs_init(void);
void RPMs_update(u16, u16, u16);
u32 get_pulse(void);
u32 get_RPS(void);

#endif

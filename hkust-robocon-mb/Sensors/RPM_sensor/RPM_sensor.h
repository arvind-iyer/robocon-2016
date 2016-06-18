#ifndef __RPM_SENSOR_H__
#define	__RPM_SENSOR_H__

#include "stm32f10x_tim.h"
#include "encoder.h"
#include "servo.h"
#include "approx_math.h"


#define RPMs_Count_TIM	TIM6
#define RPMs_In_TIM			TIM5
#define RPMs_PID_TIM		TIM9

#define RPMs_UpLim			150
#define	RPMs_DownLim		20

#define RPMs_TIM_Period	SystemCoreClock / 100000 - 1
#define	CountPerRo			6
#define diffScale				10000

enum PID_state{
	PID_OFF = 0,
	PID_ON};

void RPMs_init(void);
void RPMs_update(u16, u16, u16);
void set_tar_val(u16);
u32 get_pulse(void);
u32 get_RPS(void);
u32 get_diff(void);

#endif

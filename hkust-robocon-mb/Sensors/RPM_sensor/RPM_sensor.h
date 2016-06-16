#ifndef __RPM_SENSOR_H__
#define	__RPM_SENSOR_H__

#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

#define RPMs_Pin				GPIO_Pin_11
#define RPMs_Port				GPIOE

#define RPMs_in_TIM			TIM1
#define RPMs_Count_TIM	TIM6

#define	countPerRPM		6

void RPMs_init(void);
u16 get_RPM(void);

#endif

#ifndef _RPM_SENSOR_H_
#define _RPM_SENSOR_H_

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

#define RPMS_GPIO GPIOE
#define RPMS_PIN GPIO_Pin_11
#define RPMS_CNT_TIMx TIM1

void RPMS_init(void);
u16 get_RPMS_cnt(void);

#endif

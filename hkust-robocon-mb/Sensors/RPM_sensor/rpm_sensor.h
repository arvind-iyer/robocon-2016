#ifndef _RPM_SENSOR_H_
#define _RPM_SENSOR_H_

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

#define RPMS_GPIO 				GPIOE
#define RPMS_PIN 					GPIO_Pin_11
#define RPMS_CNT_TIMx 		TIM1

#define RPMS_UDT_TIMx			TIM6
#define RPMS_UDT_IRQn 		TIM6_IRQn
#define RPMS_UDT_Handler	TIM6_IRQHandler


void RPMS_init(void);
u32 RPMS_getVel(void);
u16 RPMS_update(void);

#endif

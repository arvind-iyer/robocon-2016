#ifndef _ENCODER_H_
#define _ENCODER_H_

// include ic library
#include "stm32f10x.h"
#include "stm32f10x_tim.h"

// define
#define ENCODER_TIMER						TIM1
#define ENCODER_TIMER_CLOCK_SOURCE			RCC_APB2Periph_TIM1						// in APB 1
#define ENCODER_TIMER_GPIO_CLOCK_SOURCE		RCC_APB2Periph_GPIOE					// in APB 2
#define ENCODER_TIMER_PORT1					GPIO_Pin_9
#define ENCODER_TIMER_PORT2					GPIO_Pin_11
#define ENCODER_TIMER_GPIOx					GPIOE
#define ENCODER_MAX_CHANGE					20000

// function declaration
void encoder_init();
void gpio_init();
void timer_init();
u32 getDistance(u32);
u32 getCount();




#endif

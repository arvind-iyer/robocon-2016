#ifndef _ENCODER_H_
#define _ENCODER_H_

// include ic library
#include "stm32f10x.h"
#include "stm32f10x_tim.h"

// define
#define ENCODER_TIMER_1						TIM1
#define ENCODER_TIMER_CLOCK_SOURCE_1			RCC_APB2Periph_TIM1						// in APB 1
#define ENCODER_TIMER_GPIO_CLOCK_SOURCE_1		RCC_APB2Periph_GPIOE					// in APB 2
#define ENCODER_TIMER_PORT1_1					GPIO_Pin_9
#define ENCODER_TIMER_PORT2_1					GPIO_Pin_11
#define ENCODER_TIMER_GPIOx_1					GPIOE
#define ENCODER_MAX_CHANGE_1					20000

#define ENCODER_TIMER_2						TIM4
#define ENCODER_TIMER_CLOCK_SOURCE_2			RCC_APB1Periph_TIM4						// in APB 1
#define ENCODER_TIMER_GPIO_CLOCK_SOURCE_2		RCC_APB2Periph_GPIOB					// in APB 2
#define ENCODER_TIMER_PORT1_2					GPIO_Pin_6
#define ENCODER_TIMER_PORT2_2					GPIO_Pin_7
#define ENCODER_TIMER_GPIOx_2					GPIOB
#define ENCODER_MAX_CHANGE_2					20000

// function declaration
void encoder_init();
void gpio_init();
void timer_init();
u32 getDistance(u32);
u32 getCount();




#endif

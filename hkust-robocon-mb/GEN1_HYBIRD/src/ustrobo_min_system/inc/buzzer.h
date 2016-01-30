#ifndef	__BUZZER_H
#define	__BUZZER_H

#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "gpio.h"
#include "ticks.h"

#define BUZZER_GPIO_PORT         GPIOB
#define BUZZER_GPIO_PIN          GPIO_Pin_0
#define BUZZER_RCC 							 RCC_APB2Periph_GPIOB

#define BUZZER_TIM							TIM3
#define BUZZER_TIM_RCC					RCC_APB1Periph_TIM3
#define BUZZER_TIM_REMAP				GPIO_FullRemap_TIM3

#define BUZZER_TIM_OC_INIT			TIM_OC3Init
#define	BUZZER_TIM_SETCOMPARE		TIM_SetCompare3

void buzzer_init(void);
void buzzer_on(void);
void buzzer_beep(u16 duration);
void buzzer_off(void);
void buzzer_update(void);

#endif	/* __BUZZER_H */

#ifndef LED_H
#define LED_H

#include "stm32f10x.h"
#include "ticks.h"

#define LED_1_GPIO				GPIOA
#define LED_2_GPIO				GPIOB
#define LED_1_Pin				GPIO_Pin_2
#define LED_2_Pin				GPIO_Pin_8

typedef enum {
	LED_1 = 1 << 0,
	LED_2 = 1 << 1,
	LED_BOTH = LED_1 | LED_2
} LED;

typedef enum {
	LED_OFF = 0,
	LED_ON	= 1
} LED_STATE;

void led_control(LED led, LED_STATE state);
void led_init(void);
void life_signal(void);
void encoder_malfunction_warning_signal(void);
void motor_overspeed_signal(void);

#endif

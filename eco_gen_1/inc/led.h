#ifndef LED_H
#define LED_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include <stdint.h>

#define LED_1  GPIO_Pin_2
#define LED_2  GPIO_Pin_3
#define LED_3  GPIO_Pin_4
#define LED_4  GPIO_Pin_5
#define LED_5  GPIO_Pin_6
#define LED_GPIO GPIOE

void led_init(void);
void LED_OFF(uint16_t gpio_pin);
void LED_ON(uint16_t gpio_pin);
void LED_ON_ALL(void);
void LED_OFF_ALL(void);

#endif

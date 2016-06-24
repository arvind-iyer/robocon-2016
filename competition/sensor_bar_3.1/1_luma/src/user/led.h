#ifndef __LED_H
#define	__LED_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

void LED_init(void);
void IndicatorControl(u8 id);

#define FD0 GPIO_Pin_13
#define FD1 GPIO_Pin_9
#define FD2 GPIO_Pin_8
#define FD3 GPIO_Pin_7
#define FD4 GPIO_Pin_6
#define FD5 GPIO_Pin_5
#define FD6 GPIO_Pin_2
#define FD7 GPIO_Pin_12
#define FD8 GPIO_Pin_11
#define FD9 GPIO_Pin_10
#define FD10 GPIO_Pin_8
#define FD11 GPIO_Pin_9
#define FD12 GPIO_Pin_8
#define FD13 GPIO_Pin_7
#define FD14 GPIO_Pin_6
#define FD15 GPIO_Pin_10

#define LED_RED GPIO_Pin_12
#define LED_GREEN GPIO_Pin_11
#define LED_BLUE GPIO_Pin_13
#endif /* __LED_H */

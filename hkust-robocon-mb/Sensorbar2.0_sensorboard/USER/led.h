#ifndef __LED_H
#define	__LED_H

#include "stm32f10x.h"

void LED_GPIO_Config(void);
void LED_Control(u8 R, u8 G, u8 B);
void IndicatorControl(u8 id);
#endif /* __LED_H */

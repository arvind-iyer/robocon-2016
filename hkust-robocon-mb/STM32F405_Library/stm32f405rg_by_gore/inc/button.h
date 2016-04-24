#ifndef	_BUTTON_H
#define	_BUTTON_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include <stdbool.h>
#include "tft_display.h"
#include "main.h"

#define BUTTON_GPIO     GPIOC
#define BUTTON_GPIO_PIN GPIO_Pin_4

void button_init(void);
u8 button_pressed( );

#endif
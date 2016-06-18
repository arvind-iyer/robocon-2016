#ifndef	_BUTTON_H
#define	_BUTTON_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include <stdbool.h>
#include "tft_display.h"
#include "main.h"

#define BUTTON_GPIO     GPIOC
#define BUTTON_GPIO_PIN GPIO_Pin_4 
#define BUTTON_RED_GPIO_PIN GPIO_Pin_0
#define BUTTON_WHITE_GPIO_PIN GPIO_Pin_1


typedef enum{
    DEFAULT_BUTTON = BUTTON_GPIO_PIN,
    BUTTON_RED = BUTTON_RED_GPIO_PIN,
    BUTTON_WHITE = BUTTON_WHITE_GPIO_PIN
}BUTTON;

void button_init(void);
u8 button_pressed(BUTTON button);

#endif

#ifndef	_BUTTON_H
#define	_BUTTON_H

#include "stm32f4xx.h"
#include <stdbool.h>
#include "tm_stm32f4_gpio.h"
#include "lcd_main.h"
#include "main.h"

#define BUTTON_ONE_PIN  	GPIO_Pin_4
#define BUTTON_JS_GPIO 			GPIOC

typedef enum{
	BUTTON_PIN = BUTTON_ONE_PIN, 
}BUTTON;

void button_init(void);
bool button_pressed(BUTTON button);

#endif /* __LED_H */

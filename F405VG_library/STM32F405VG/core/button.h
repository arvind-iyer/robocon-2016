#ifndef	_BUTTON_H
#define	_BUTTON_H

#include "stm32f4xx.h"
#include <stdbool.h>
#include "gpio.h"
#include "lcd_main.h"
#include "main.h"

#define BUTTONS_COUNT 8

typedef enum{
	BUTTON_1 = 0, 
	BUTTON_2,
	BUTTON_3,
	BUTTON_4,
	BUTTON_5,
	BUTTON_6,
	BUTTON_7,
	BUTTON_8
}BUTTON_ID;

void button_init(void);
bool button_pressed(BUTTON_ID button_id);

#endif /* __LED_H */

#ifndef	_BUTTON_H
#define	_BUTTON_H

#include "stm32f4xx.h"
#include <stdbool.h>
#include "tm_stm32f4_gpio.h"
#include "lcd_main.h"
#include "main.h"

#define BUTTON_1_PIN  				GPIO_Pin_10
#define BUTTON_2_PIN  				GPIO_Pin_11
//#define BUTTON_3_PIN					GPIO_Pin_12 //Removed: causing crash
#define BUTTON_4_PIN  				GPIO_Pin_2
#define BUTTON_1_3_GPIO 			GPIOC
#define BUTTON_4_GPIO 				GPIOD

//external button
#define BUTTON_5_PIN  				GPIO_Pin_4
#define BUTTON_6_PIN  				GPIO_Pin_5
#define BUTTON_7_PIN					GPIO_Pin_6
#define BUTTON_8_PIN  				GPIO_Pin_7
#define BUTTON_5_8_GPIO 			GPIOA

typedef enum{
	BUT_1 = BUTTON_1_PIN, 
	BUT_2 = BUTTON_2_PIN, 
//	BUT_3 = BUTTON_3_PIN, 
	BUT_4 = BUTTON_4_PIN, 
	BUT_5 = BUTTON_5_PIN, 
	BUT_6 = BUTTON_6_PIN, 
	BUT_7 = BUTTON_7_PIN, 
	BUT_8 = BUTTON_8_PIN
}BUTTON;

void button_init(void);
bool button_pressed(BUTTON button);

#endif /* __LED_H */

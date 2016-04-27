#ifndef	_BUTTON_H
#define	_BUTTON_H

#include "stm32f4xx.h"
#include <stdbool.h>
#include "tm_stm32f4_gpio.h"
#include "lcd_main.h"
#include "main.h"

//Rotate the joystick to match the lcd orientation
#if ORIENTATION_SETTING == 0 //PIN ON TOP
	#define BUTTON_JS_UP_PIN  			GPIO_Pin_6
	#define BUTTON_JS_DOWN_PIN  		GPIO_Pin_3
	#define BUTTON_JS_LEFT_PIN  		GPIO_Pin_4
	#define BUTTON_JS_RIGHT_PIN  		GPIO_Pin_5
#elif ORIENTATION_SETTING == 2 //PIN ON BOTTOM
	#define BUTTON_JS_UP_PIN  			GPIO_Pin_3
	#define BUTTON_JS_DOWN_PIN  		GPIO_Pin_6
	#define BUTTON_JS_LEFT_PIN  		GPIO_Pin_5
	#define BUTTON_JS_RIGHT_PIN  		GPIO_Pin_4
#elif ORIENTATION_SETTING == 1 //PIN ON LEFT
	#define BUTTON_JS_UP_PIN  			GPIO_Pin_5
	#define BUTTON_JS_DOWN_PIN  		GPIO_Pin_4
	#define BUTTON_JS_LEFT_PIN  		GPIO_Pin_6
	#define BUTTON_JS_RIGHT_PIN  		GPIO_Pin_3
#elif ORIENTATION_SETTING == 3 //PIN ON RIGHT
	#define BUTTON_JS_UP_PIN  			GPIO_Pin_4
	#define BUTTON_JS_DOWN_PIN  		GPIO_Pin_5
	#define BUTTON_JS_LEFT_PIN  		GPIO_Pin_3
	#define BUTTON_JS_RIGHT_PIN  		GPIO_Pin_6
#else
	#error ORIENTATION INCORRECTLY SET
#endif

#define BUTTON_JS_CENTRE_PIN  	GPIO_Pin_7
#define BUTTON_JS_GPIO 			GPIOB

typedef enum{
	BUTTON_JS_UP = BUTTON_JS_UP_PIN, 
	BUTTON_JS_DOWN = BUTTON_JS_DOWN_PIN,
	BUTTON_JS_LEFT = BUTTON_JS_LEFT_PIN,
	BUTTON_JS_RIGHT = BUTTON_JS_RIGHT_PIN,
	BUTTON_JS_CENTRE = BUTTON_JS_CENTRE_PIN
}BUTTON;

void button_init(void);
bool button_pressed(BUTTON button);

#endif /* __LED_H */

#ifndef _MAIN_H
#define _MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "led.h"

//	PIN_ON_TOP = 		0
//	PIN_ON_LEFT = 	1
//	PIN_ON_BOTTOM =	2
//	PIN_ON_RIGHT = 	3
#define ORIENTATION_SETTING 2

#include "stm32f4xx_gpio.h"
#include "servo.h"
#include "ticks.h"
#include "usart.h"
#include "approx_math.h"
#include "can_protocol.h"
#include "encoder.h"
#include "button.h"
#include "buzzer.h"
#include "buzzer_song.h"
#include "lcd_main.h"
#include "adc.h"

#endif 

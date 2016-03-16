#ifndef _PATH_RIVER_H
#define _PATH_RIVER_H

#include "stm32f4xx.h"
#include "main.h"
#include "led.h"
#include "lcd_main.h"
#include "usart.h"
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "tm_stm32f4_servo.h"
#include "servo_targeting.h"
#include "path.h"

#define IR_GPIO GPIOE
#define IR_1_Pin GPIO_Pin_10
#define IR_2_Pin GPIO_Pin_11

#define IR_BUFFER_LENGTH 5

void path_river_init(void);
GAME_STAGE path_river_update(void);

#endif

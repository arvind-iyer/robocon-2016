#ifndef _PATH_DOWN_H
#define _PATH_DOWN_H

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

void path_down_init(void);
GAME_STAGE path_down_update(void);

#endif

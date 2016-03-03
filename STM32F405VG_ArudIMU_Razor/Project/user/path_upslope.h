#ifndef _PATH_UP_H
#define _PATH_UP_H

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

#define ROLLING_PITCH_SIZE 5
#define PROGRESS_TICKS_MAX 10

void path_up_init(float* ypr);
GAME_STAGE path_up_update(void);

#endif

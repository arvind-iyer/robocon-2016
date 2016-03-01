#ifndef _PATH_H
#define _PATH_H

#include "stm32f4xx.h"
#include "led.h"
#include "lcd_main.h"
#include "usart.h"
#include "main.h"
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "tm_stm32f4_servo.h"
#include "servo_targeting.h"

#define ROLLING_PITCH_SIZE 3
#define PROGRESS_COUNT_MAX 1

void path_init(float* ypr);
void path_update(void);

#endif

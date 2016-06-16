#ifndef _PATH_DOWN_H
#define _PATH_DOWN_H

#include "stm32f4xx.h"
#include "main.h"
#include "led.h"
#include "lcd_main.h"
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "imu_targeting.h"
#include "sensor_bar.h"
#include "servo_interface.h"
#include "path.h"
#include "path_downslope.h"

#define DOWN_SB_INC_PWM 4800
#define DOWN_SB_DEC_PWM 4800
#define DOWN_SLOPE_MIN_DIS 2000
#define SB_SHIFT_AFTER_THIRD 1

void path_down_reset(void);
GAME_STAGE path_down_update(void);

#endif

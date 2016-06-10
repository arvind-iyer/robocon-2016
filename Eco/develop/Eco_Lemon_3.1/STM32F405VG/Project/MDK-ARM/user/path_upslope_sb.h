#ifndef _PATH_UP_SB_H
#define _PATH_UP_SB_H

#include "stm32f4xx.h"
#include "main.h"
#include "led.h"
#include "lcd_main.h"
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "sensor_bar.h"
#include "servo_interface.h"
#include "path.h"
#include "quick_math.h"
#include "encoder.h"

#define SLOPE_SECTION_ENC 3000
#define SLOPE_SB_INC_PWM 3000 //4500
#define SLOPE_SB_DEC_PWM 3000 //4500
#define RIVER_FIRST_SENSOR_BAR_BIAS 2
#define RIVER_90_TURN_ENC_CONSTANT 3500
#define RIVER_90_MAX_TURN_UNTIL 500

void path_up_sb_init(u8 stage);
GAME_STAGE path_up_sb_update(void);

#endif

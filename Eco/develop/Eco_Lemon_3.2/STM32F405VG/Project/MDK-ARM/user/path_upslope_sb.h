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

#define SLOPE_SECTION_DIS 350 //in mm
#define SLOPE_SB_INC_PWM 3800 //3000
#define SLOPE_SB_DEC_PWM 3800 //3000
#define RIVER_FIRST_SENSOR_BAR_BIAS 2

//use the formula enc=y/(x+vel)
#define RIVER_90_TURN_ENC_X (800/11) //(800/11)
#define RIVER_90_TURN_ENC_Y (36000000/8)//(36000000/11) //(24000000/11)
#define RIVER_90_MAX_TURN_UNTIL 500

void path_up_sb_init(u8 stage);
GAME_STAGE path_up_sb_update(void);

#endif

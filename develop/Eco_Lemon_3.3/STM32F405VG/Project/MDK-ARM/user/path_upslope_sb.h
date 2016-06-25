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

//Buffer for changing background color
#define SLOPE_SECTION_DIS 350 //in mm
//Max and min PWM increase for tracking lines in upslope
#define SLOPE_SB_INC_PWM 3800 //3000
#define SLOPE_SB_DEC_PWM 3800 //3000
//New middle position bias after reaching first island
#define RIVER_FIRST_SENSOR_BAR_BIAS 2

//#define RIVER_90_TURN_ENC_X (800/11) //(800/11)
//use the formula enc=y/(x+vel)
//#define RIVER_90_TURN_ENC_Y (22000000/10) //(36000000/8) //(36000000/11)

//Encoder count after reaching third orange
#ifdef RED_FIELD
	#define RIVER_90_TURN_DIS 100
#else
	#define RIVER_90_TURN_DIS 100
#endif

//Servo PWM during 90-turn
#ifdef BLUE_FIELD
	#define SERVO_RIVER_PWM (SERVO_MED_PWM + 3500)
#else
	#define SERVO_RIVER_PWM (SERVO_MED_PWM - 3500)
#endif

//Target IMU angle to finish 90-turn
#define RIVER_90_MAX_TURN_UNTIL 500

void path_up_sb_init(u8 stage);
GAME_STAGE path_up_sb_update(void);

#endif

#ifndef _PATH_RIVER_H
#define _PATH_RIVER_H

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
#include "MTi_adp.h"
#include "adc_ir.h"

#ifdef BLUE_FIELD
	#define FIRST_IR_ID 0
	#define SECOND_IR_ID 1
#else
	#define FIRST_IR_ID 1
	#define SECOND_IR_ID 0
#endif

#ifdef BLUE_FIELD
	#define RIVER_STRAIGHT_YAW (900+40)
	#define TURNING_AFTER_THIRD_ISLAND (-20)
#else
	#define RIVER_STRAIGHT_YAW (-900-40)
	#define TURNING_AFTER_THIRD_ISLAND (20)
#endif

#define IR_BUFFER_LENGTH 8
#define IR_ENCODER_DIS 350 //in mm, between end and next start
#define DIS_AFTER_THIRD_ISLAND 0 //100 //200 //60//in mm
#define RIVER_SB_INC_PWM 5000
#define RIVER_SB_DEC_PWM 5000

void path_river_init(s16 straight_yaw);
GAME_STAGE path_river_update(void);

#endif

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

//Straight yaw: Target angle in IMU tracking between first and third island
//Turning: Target angle in IMU tracking after third island, if no white line is detected
#ifdef BLUE_FIELD
	#define RIVER_STRAIGHT_YAW (900+40)
	#define TURNING_AFTER_THIRD_ISLAND (-20)
#else
	#define RIVER_STRAIGHT_YAW (-900-40)
	#define TURNING_AFTER_THIRD_ISLAND (20)
#endif

#define IR_BUFFER_LENGTH 8
#define IR_ENCODER_DIS 350 //in mm, between end and next start
#define DIS_AFTER_THIRD_ISLAND 0 //Encoder count to enable sensor bar after senses third island

//Max and min servo pwm in line tracking
#define RIVER_SB_INC_PWM 5000
#define RIVER_SB_DEC_PWM 5000

//Static degree bias after third island
#ifdef BLUE_FIELD
	#define DEGREE_AFTER_THRID_ISLAND 250
#else
	#define DEGREE_AFTER_THRID_ISLAND -250
#endif

void path_river_init(s16 straight_yaw);
GAME_STAGE path_river_update(void);

#endif

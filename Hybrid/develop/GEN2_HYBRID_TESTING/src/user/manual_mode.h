#ifndef _MANUAL_MODE_H
#define _MANUAL_MODE_H

#include "stm32f10x.h"
#include <stdlib.h>
#include <stdbool.h>

#include "approx_math.h"
#include "xbc_mb.h"
#include "button.h"
#include "servo.h"
#include "pneumatic.h"
#include "buzzer.h"
#include "led.h"
#include "auto_mode.h"
#include "gyro.h"
#include "xbc_control.h"
#include "main.h"
#include "robot_control.h"
#include "manual_track.h"
#include "semi_auto.h"
#include "acc_module.h"
#include "quick_math.h"
#include "adc_ir.h"

//#define BLUE_FIELD
#define RED_FIELD

#ifdef RED_FIELD
	#ifdef BLUE_FIELD
		#error FIELD COLOR BOTH DEFINED
	#endif
#else
	#ifndef BLUE_FIELD
		#error FIELD COLOR NOT YET DEFINED
	#endif
#endif

//Scaled by 1000
#define ANGLE_PID_P 300
#define ANGLE_PID_I 10
#define ANGLE_PID_D 0
#define ANGLE_PID_MAX 400000

#define IR_ORIGINAL_TARGET_DIS 45 //in mm
#define HIT_BOX_PORT PE3
 
//Laser avoid hitting
#define LASER_RETREAT_AVG_DIS 200
#define LASER_RETREAT_ONE_SIDE_DIS 180
#define LASER_RETREAT_SPEED 560
#define LASER_RETRAET_P 2000 //Scaled by 1000
#define LASER_RETREAT_ROTATE 350

#ifdef BLUE_FIELD
	#define FRONT_LASER_ID 2
	#define BACK_LASER_ID 0
#else
	#define FRONT_LASER_ID 0
	#define BACK_LASER_ID 2
#endif

//Make deceleration faster than acceleration
#define BASE_ACC_CONSTANT 7000 //4000 //3200 //Scaled by 1000
#define BASE_DEC_CONSTANT 8400 //5600 //3400 //Scaled by 1000
#define BASE_VEL_JOYSTICK_GAIN 2900 //Scaled by 1000
#define ROTATE_ACC_CONSTANT 10400 //3500 //Scaled by 1000 
#define ROTATE_DEC_CONSTANT 13100 //3500 //Scaled by 1000 

#define BRUSHLESS_POWER_STEP 5
#define BRUSHLESS_SERVO_SMALL_STEP 1
#define BRUSHLESS_SERVO_LARGE_STEP 3
#define BRUSHLESS_START_CONTIN_TICKS 750
#define BRUSHLESS_KEEP_CONTIN_TICKS 350

#define GRIPPER_STATES_NO 4
#define GRIPPER_TICKS_THRESHOLD 50

#define CLIMBING_TICKS_LIMIT 1000
#define CLIMBING_BRUSHLESS_POWER 50
#define PUTTING_PROPELLER_UP_DELAY 1000
#define PUTTING_PROPELLER_PUSH_DELAY 1000
#define PUTTING_PROPELLER_UNCLAW_DELAY 1000
#define PUTTING_PROPELLER_RETRACT_DELAY 1000
#define PUTTING_PROPELLER_YEAH_DELAY 1000

#ifdef BLUE_FIELD
	#define THIS_GRIPPER GRIPPER_1
	#define CLIMBING_BRUSHLESS_ANGLE -5
#else
	#define THIS_GRIPPER GRIPPER_2
	#define CLIMBING_BRUSHLESS_ANGLE 5
#endif

extern bool gripper_extended, gripper_clawed, gripper_down;

void manual_reset(void);
void manual_init(void);
void manual_interval_update(void);
void manual_fast_update(void);
void manual_first_control_update(void);
void manual_second_control_update(void);
void manual_controls_update(void);
void manual_vel_set_zero(void);
void manual_control_brushless_update(void);
void manual_auto_gointo_pole(void);

#endif
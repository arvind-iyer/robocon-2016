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
#include "climbing.h"

#define BLUE_FIELD
//#define RED_FIELD

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
#define HIT_BOX_L_PORT &PE3
#define HIT_BOX_R_PORT &PE9
 
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
#define BASE_X_ACC_CONSTANT 10 //3500 //4000 //3200 
#define BASE_X_DEC_CONSTANT 14 //4200 //5600 //3400 
#define BASE_Y_ACC_CONSTANT 16 //3500 //4000 //3200 
#define BASE_Y_DEC_CONSTANT 22 //4200 //5600 //3400 

#define BASE_VEL_JOYSTICK_GAIN 1900 //Scaled by 1000
#define ROTATE_ACC_CONSTANT 25 //5020 //3500 
#define ROTATE_DEC_CONSTANT 35 //6100 //3500 

#define BRUSHLESS_POWER_STEP 3
#define BRUSHLESS_SERVO_SMALL_STEP 1
#define BRUSHLESS_SERVO_LARGE_STEP 3
#define BRUSHLESS_START_CONTIN_TICKS 750
#define BRUSHLESS_KEEP_CONTIN_TICKS 200

#define GRIPPER_STATES_NO 4
#define GRIPPER_TICKS_THRESHOLD 50

#ifdef BLUE_FIELD
	#define THIS_GRIPPER GRIPPER_1
#else
	#define THIS_GRIPPER GRIPPER_2
#endif

extern bool gripper_extended, gripper_clawed;
extern GRIPPER_UP_STATE gripper_down;
extern s16 brushless_servo_val;
extern u16 brushless_power_percent;

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

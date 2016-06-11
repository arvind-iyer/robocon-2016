#ifndef _MANUAL_MODE_H
#define _MANUAL_MODE_H

#include "stm32f10x.h"
#include <stdlib.h>
#include <stdbool.h>

#include "can_motor.h"
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
#include "quick_math.h"

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

#define ARM_IR_PORT PE0
#define HIT_BOX_PORT PE3

//Make deceleration faster than acceleration
#define BASE_ACC_CONSTANT 2700 //3500 //Scaled by 1000
#define BASE_DEC_CONSTANT 2800 //8500 //Scaled by 1000
#define BASE_VEL_JOYSTICK_GAIN 1300 //Scaled by 1000
#define ROTATE_ACCEL_CONSTANT 3500 //14000 //Scaled by 1000 
#define MOTOR_MAX_MY_BOUND 150 //<150

#define BRUSHLESS_POWER_STEP 5
#define BRUSHLESS_SERVO_SMALL_STEP 2
#define BRUSHLESS_SERVO_LARGE_STEP 5
#define BRUSHLESS_START_CONTIN_TICKS 500
#define BRUSHLESS_KEEP_CONTIN_TICKS 200

#define GRIPPER_STATES_NO 4
#define GRIPPER_TICKS_THRESHOLD 50

#define CLIMBING_TICKS_LIMIT 500
#define CLIMBING_BRUSHLESS_POWER 70
#define PUTTING_PROPELLER_PUSH_DELAY 200
#define PUTTING_PROPELLER_UNCLAW_DELAY 400
#define PUTTING_PROPELLER_RETRACT_DELAY 400
#define PUTTING_PROPELLER_YEAH_DELAY 500

#ifdef RED_FIELD
	#define THIS_GRIPPER GRIPPER_1
#else
	#define THIS_GRIPPER GRIPPER_2
#endif

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

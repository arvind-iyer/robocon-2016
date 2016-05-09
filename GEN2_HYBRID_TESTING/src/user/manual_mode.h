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
#include "pid_lock.h"
#include "xbc_control.h"
#include "main.h"
#include "robot_control.h"

//Scaled by 1000
#define ANGLE_PID_P 300
#define ANGLE_PID_I 10
#define ANGLE_PID_D 0
#define ANGLE_PID_MAX 400000

#define BASE_ACCEL_CONSTANT 1500 //Scaled by 1000
#define ROTATE_ACCEL_CONSTANT 1500 //Scaled by 1000

#define BRUSHLESS_SERVO_STEP 5

#define GRIPPER_STATES_NO 4
#define GRIPPER_TICKS_THRESHOLD 50

#define ENCODER_READING_ARRAY_SIZE 100

void manual_reset(void);
void manual_init(void);
void manual_interval_update(void);
void manual_fast_update(void);
void manual_controls_update(void);
void manual_vel_set_zero(void);

#endif

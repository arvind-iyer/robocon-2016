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

#define LOW_SPEED_ACC 17
#define HIGH_SPEED_ACC 30
#define LOW_SPEED_THRESHOLD 100

//Scaled by 1000
#define ANGLE_PID_P 3000
#define ANGLE_PID_I 0
#define ANGLE_PID_D 0
#define ANGLE_PID_MAX 600

#define BRUSHLESS_LOCK_TIMEOUT 5000

#define ENCODER_READING_ARRAY_SIZE 100

void manual_reset(void);
void manual_init(void);
void manual_interval_update(void);
void manual_fast_update(void);
void manual_vel_set_zero(void);

#endif

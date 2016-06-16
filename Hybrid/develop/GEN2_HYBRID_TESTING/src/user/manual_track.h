#ifndef _MANUAL_TRACK_H
#define _MANUAL_TRACK_H

#include "stm32f10x.h"
#include <stdlib.h>
#include <stdbool.h>

#include "approx_math.h"
#include "ticks.h"
#include "xbc_mb.h"
#include "button.h"
#include "servo.h"
#include "buzzer.h"
#include "gyro.h"
#include "xbc_control.h"
#include "main.h"
#include "robot_control.h"
#include "laser_sensor.h"
#include "quick_math.h"
#include "manual_mode.h"

#define WALL_1_LIMIT_SWITCH &PE9
#define WALL_2_LIMIT_SWITCH &PE10
#define POLE_LIMIT_SWITCH &PE2
#define TICKS_AFTER_HIT_POLE 800

#define RIVER_ROTATE_P 1000 //Scaled by 1000 
#define RIVER_ROTATE_D 1400 //Scaled by 1000
#define RIVER_ROTATE_MAX 450

#define LASER_START_BACKING_OFF 6000
#define LASER_BACK_OFF_DISTANCE 0 //250
#define LASER_OFF_MIN_DISTANCE 5500

#define LASER_ROTATE_P 1800 //Scaled by 1000
#define LASER_ROTATE_D 2100 //Scaled by 1000

#define LASER_PERPEND_P 5000 //Scaled by 1000
#define LASER_TARGET_RANGE 250 //In mm
#define LASER_OUT_DISTANCE 800
#define LASER_MAX_PARA_SPEED 300

#define LASER_PARA_P 880 //Scaled by 1000
#define LASER_PARA_CONSTANT 880

#define LIMIT_START_DECEL_Y 4500
#define LIMIT_END_DECEL_Y 5200
#define LIMIT_NO_Y_TOLER 4
#define LIMIT_NO_Y_BUFFER 50

#define LIMIT_ROTATE_BIGGER 450
#define LIMIT_ROTATE_SMALLER 100

#define LIMIT_PARA_CONSTANT 750
#define LIMIT_PARA_SLOW_CONSTANT 380
#define LIMIT_ACCEL_TIME 1000 //In ms

#define LIMIT_PERPEND_NORM 20
#define LIMIT_PERPEND_FAST 80

#define TRACK_ACC_CONSTANT 5500 //Scaled by 1000

extern u32 laser_range[2];

bool laser_manual_update(s32 motor_vel[3]);
void limit_manual_init(void);
u8 limit_manual_update(s32 motor_vel[3]);
s32 river_rotate_update(s32 target);

#endif

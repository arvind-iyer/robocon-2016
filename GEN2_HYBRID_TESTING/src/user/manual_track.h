#ifndef _MANUAL_TRACK_H
#define _MANUAL_TRACK_H

#include "stm32f10x.h"
#include <stdlib.h>
#include <stdbool.h>

#include "can_motor.h"
#include "approx_math.h"
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

#define RIVER_ROTATE_P 800 //Scaled by 1000 
#define RIVER_ROTATE_D 1400 //Scaled by 1000

#define LASER_START_BACKING_OFF 6200
#define LASER_BACK_OFF_DISTANCE 250

#define LASER_ROTATE_P 650 //Scaled by 1000
#define LASER_ROTATE_D 2100 //Scaled by 1000

#define LASER_PERPEND_P 1000 //Scaled by 1000
#define LASER_TARGET_RANGE 350 //In mm

#define LASER_PARA_P 550 //Scaled by 1000
#define LASER_PARA_CONSTANT 800

void laser_manual_update(s32 motor_vel[3], s32* rotate);
s32 river_rotate_update(s32 target);

#endif

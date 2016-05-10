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
#include "led.h"
#include "auto_mode.h"
#include "gyro.h"
#include "xbc_control.h"
#include "main.h"
#include "robot_control.h"
#include "laser_sensor.h"
#include "quick_math.h"

#define LASER_ROTATE_P 400 //Scaled by 1000

#define LASER_PERPEND_P 600 //Scaled by 1000
#define LASER_TARGET_RANGE 240 //In mm

#define LASER_PARA_P 450 //Scaled by 1000

void laser_manual_update(s32 motor_vel[3], s32* rotate);

#endif

#ifndef _MANUAL_MOVE_H
#define _MANUAL_MOVE_H

#include "stm32f10x.h"
#include "can_motor.h"
#include "approx_math.h"
#include "xbc_mb.h"
#include "button.h"
#include "servo.h"
#include "ticks.h"

#define ACC_STEPPING 3
#define ACC_THRESHOLD 15
#define CLIMBING_SPEED 80

#define BURSHLESS_MIN 400
#define BURSHLESS_MAX 1050

void manual_init();
void climb_continue();
void stop_climbing();
void manual_control_update();
void set_global_movement(s32 angle, s32 speed, s32 rotate);
void brushless_control(s16 value);

#endif
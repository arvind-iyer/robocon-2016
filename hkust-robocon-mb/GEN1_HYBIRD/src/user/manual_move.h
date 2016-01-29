#ifndef _MANUAL_MOVE_H
#define _MANUAL_MOVE_H

#include "stm32f10x.h"
#include "can_motor.h"
#include "approx_math.h"
#include "xbc_mb.h"
#include "button.h"
#include "servo.h"
#include "ticks.h"
#include "pneumatic.h"

#define ACC_THRESHOLD 1
#define CLIMBING_SPEED 80

#define BRUSHLESS_MIN 400
#define BRUSHLESS_MAX 1050

typedef enum {
	MANUAL_CONTROL = 0,
	AUTO_MODE = 1
}CONTROL_STATE;

typedef enum {
	LOCKED = 0,
	UNLOCKED = 1
}LOCK_STATE;

void manual_init();
void climb_continue();
void stop_climbing();
CONTROL_STATE manual_control_update();
void brushless_control(s16 value);

#endif
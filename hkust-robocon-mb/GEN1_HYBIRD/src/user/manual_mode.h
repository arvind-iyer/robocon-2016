#ifndef _MANUAL_MODE_H
#define _MANUAL_MODE_H

#include "stm32f10x.h"
#include "can_motor.h"
#include "approx_math.h"
#include "xbc_mb.h"
#include "button.h"
#include "servo.h"
#include "ticks.h"
#include "pneumatic.h"
#include "buzzer.h"

#define ACC_THRESHOLD 1
#define CLIMBING_SPEED 1799
#define DESCEND_SPEED -600

#define BRUSHLESS_MIN 400
#define BRUSHLESS_MED_HIGH 700
#define BRUSHLESS_MAX 1050

typedef enum {
	MANUAL_CONTROL = 0,
	AUTO_MODE = 1
}CONTROL_STATE;

typedef enum {
	LOCKED = 0,
	UNLOCKED = 1
}LOCK_STATE;

void manual_reset(void);
void manual_init(void);
void climb_continue(void);
void stop_climbing(void);
CONTROL_STATE manual_control_update(void);
void brushless_control(s16 value);

#endif
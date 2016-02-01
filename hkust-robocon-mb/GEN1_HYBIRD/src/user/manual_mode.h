#ifndef _MANUAL_MODE_H
#define _MANUAL_MODE_H

#include "stm32f10x.h"
#include <stdlib.h>
#include "can_motor.h"
#include "approx_math.h"
#include "xbc_mb.h"
#include "button.h"
#include "servo.h"
#include "ticks.h"
#include "pneumatic.h"
#include "buzzer.h"
#include "led.h"
#include "auto_mode.h"
#include "gyro.h"
#include "pid_lock.h"
#include "xbc_control.h"

#define ACC_THRESHOLD 1
#define CLIMBING_SPEED 1799
#define DESCEND_SPEED -500

#define BRUSHLESS_MIN 400
#define BRUSHLESS_MED_HIGH 800
#define BRUSHLESS_MAX 1050
#define BRUSHLESS_COUNT 2

typedef enum{
	BRUSHLESS_1 = 0,
	BRUSHLESS_2 = 1
}BRUSHLESS_ID;

void manual_reset(void);
void manual_init(void);
void manual_interval_update(void);
void manual_fast_update(void);
void manual_emergency_relax(void);
void manual_emergency_stop(void);

#endif
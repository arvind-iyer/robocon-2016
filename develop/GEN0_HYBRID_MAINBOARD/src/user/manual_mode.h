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

#define ACC_THRESHOLD 2
#define ACC_RATE 45 // FROM 1 to 1000, 1000 is instant change speed
#define XBC_BUTTON_MIN_PRESSED 6

void manual_reset(void);
void manual_init(void);
void manual_interval_update(void);
void manual_fast_update(void);

#endif

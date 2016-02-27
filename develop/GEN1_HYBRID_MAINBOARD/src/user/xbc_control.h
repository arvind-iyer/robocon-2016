#ifndef _XBC_CONTROL_H
#define _XBC_CONTROL_H

#include "stm32f10x.h"
#include "can_motor.h"
#include "servo.h"
#include "xbc_mb.h"
#include "can_xbc_mb.h"
#include "button.h"
#include "ticks.h"
#include "buzzer.h"
#include "gyro.h"
#include "pid_lock.h"
#include "led.h"
#include "lcd_main.h"
#include "robot_control.h"

typedef enum {
	MANUAL_MODE = 0,
	AUTO_MODE = 1
}CONTROL_STATE;

typedef enum {
	LOCKED = 0,
	UNLOCKED = 1
}LOCK_STATE;

void xbc_global_update(void);
CONTROL_STATE get_control_state(void);
LOCK_STATE get_emergency_lock(void);

#endif

#ifndef _CLIMBING_H
#define _CLIMBING_H

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

#define CLIMBING_TICKS_LIMIT 1000
#define CLIMBING_BRUSHLESS_P 2000 //Scaled by 1000
#define CLIMBING_BRUSHLESS_ANGLE 35 //5

#define PUTTING_PROPELLER_UP_DELAY 800
#define PUTTING_PROPELLER_PUSH_DELAY 800
#define PUTTING_PROPELLER_UNCLAW_DELAY 800
#define PUTTING_PROPELLER_RETRACT_DELAY 800
#define PUTTING_PROPELLER_YEAH_DELAY 800

u8 climbing_update(void);
void climbing_init(void);

#endif

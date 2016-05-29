#ifndef _SEMI_AUTO_H
#define _SEMI_AUTO_H

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

void limit_sa_init(void);
u8 limit_sa_update(s32 motor_vel[3], s32* rotate);

#endif

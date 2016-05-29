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

#define LM_SA_ROTATE_P 250
#define LM_SA_ROTATE_D 1100
#define LM_SA_CALI_TIME 800

#define LM_SA_APP_FAST_SPEED 800
#define LM_SA_APP_SLOW_SPEED 300
#define LM_SA_TOWARDS_SPEED 220
#define LM_SA_APP_START_ACCEL 5500
#define LM_SA_APP_END_ACCEL 6200

#define LM_SA_STR_ANGLE_P 400
#define LM_SA_STR_FAST 1000
#define LM_SA_STR_SLOW 450
#define LM_SA_ACC_START_Y 3000
#define LM_SA_ACC_END_Y 4500
#define LM_SA_STR_X_P 1200

void limit_sa_init(void);
void limit_sa_approach_init(void);
u8 limit_sa_approach(s32 motor_vel[3], s32* rotate);
u8 limit_sa_update(s32 motor_vel[3], s32* rotate);
u8 sa_str_update(s32 motor_vel[3], s32* rotate);

#endif

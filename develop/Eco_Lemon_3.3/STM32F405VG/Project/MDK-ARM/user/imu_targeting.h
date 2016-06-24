#ifndef _IMU_TARGETING_H
#define _IMU_TARGETING_H

#include "stm32f4xx.h"
#include "led.h"
#include "lcd_main.h"
#include "main.h"
#include <stdbool.h>
#include <math.h>
#include "servo.h"
#include "angle_range.h"
#include "servo_interface.h"
#include "quick_math.h"

#define IMU_PID_P 380 //Scaled by 100

extern s16 target_yaw;

void targeting_update(s16 current_yaw);
void set_target(s16 in_target_yaw);
s16 targeting_pid(s16 current_yaw);

#endif

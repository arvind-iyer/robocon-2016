#ifndef _IMU_TARGETING_H
#define _IMU_TARGETING_H

#include "stm32f4xx.h"
#include "led.h"
#include "lcd_main.h"
#include "main.h"
#include <stdbool.h>
#include <math.h>
#include "servo.h"
#include "imu_math.h"
#include "servo_interface.h"
#include "quick_math.h"

//Scaled by 1000
#define SERVO_Kp 3000.0f

extern s16 target_yaw;

void targeting_update(s16 current_yaw);
void set_target(s16 in_target_yaw);
s16 targeting_pid(s16 current_yaw);

#endif

#ifndef _SERVO_TARGETING_H
#define _SERVO_TARGETING_H

#include "stm32f4xx.h"
#include "led.h"
#include "lcd_main.h"
#include "usart.h"
#include "main.h"
#include <stdbool.h>
#include <math.h>
#include "tm_stm32f4_servo.h"
#include "imu_math.h"

//Scaled by 1000
#define SERVO_Kp 3000.0f

#define SERVO_MED_DEG 90
#define SERVO_INC_DEG 40
#define SERVO_DEC_DEG 40
#define SERVO_MAX_DEG SERVO_MED_DEG + SERVO_INC_DEG
#define SERVO_MIN_DEG SERVO_MED_DEG - SERVO_DEC_DEG

extern float target_yaw;
extern TM_SERVO_t dragon_servo;

void servo_init(void);
void targeting_update(float current_yaw);
void set_target(float in_target_yaw);

#endif

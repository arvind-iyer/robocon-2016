#ifndef _SERVO_TARGETING_H
#define _SERVO_TARGETING_H

#include "stm32f4xx.h"
#include "led.h"
#include "lcd_main.h"
#include "usart.h"
#include "main.h"
#include <stdbool.h>
#include <math.h>
#include "servo.h"
#include "imu_math.h"

//Scaled by 1000
#define SERVO_Kp 3000.0f

#define SERVO_MED_PWM 1500
#define SERVO_INC_PWM 400
#define SERVO_DEC_PWM 400
#define SERVO_MAX_PWM SERVO_MED_PWM + SERVO_INC_PWM
#define SERVO_MIN_PWM SERVO_MED_PWM - SERVO_DEC_PWM

#define DRAGON_SERVO SERVO1

extern float target_yaw;

void servo_init(void);
void targeting_update(float current_yaw);
void force_set_angle(float angle);
void set_target(float in_target_yaw);
void enable_sensor_bar(u16 new_trust, u8 new_power, u16 new_sensor_bar_Kp);
float targeting_pid(float current_yaw);
void disable_sensor_bar(void);

#endif

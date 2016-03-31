#ifndef SERVO_INTERFACE_H
#define SERVO_INTERFACE_H

#include "stm32f4xx.h"
#include "servo.h"
#include "quick_math.h"
#include "lcd_main.h"

#define SERVO_MED_PWM 15000
#define SERVO_INC_PWM 4000
#define SERVO_DEC_PWM 4000
#define SERVO_MAX_PWM SERVO_MED_PWM + SERVO_INC_PWM
#define SERVO_MIN_PWM SERVO_MED_PWM - SERVO_DEC_PWM

#define DRAGON_SERVO SERVO2

extern s16 current_servo_pwm;
extern s16 pending_servo_pwm;

//All pwm and angle are scaled by 10
void si_init(void);
void si_set_st_deg_bias(s16 angle_bias);
void si_set_st_pwm_bias(s16 pwm_bias);
void si_add_deg_bias(s16 angle_bias);
void si_add_pwm_bias(s16 pwm_bias);
void si_set_deg(u16 deg);
void si_set_pwm(u16 pwm);
void si_execute(void);
void si_clear(void);
void si_clear_static(void);
	
#endif

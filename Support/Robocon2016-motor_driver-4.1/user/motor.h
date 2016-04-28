#ifndef __MOTOR_H_
#define __MOTOR_H_

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"	  
#include "stm32f10x_tim.h"

#define PWM_GPIO GPIOB
#define PWM_PIN GPIO_Pin_0
#define DIR_GPIO GPIOB
#define DIR_PIN GPIO_Pin_1
#define MOTOR_TIM TIM3

void motor_init(void);
void motor_control(u8 dir,u16 magnitude);//dir can be 1 or 0 , pwm can be 1 or 2  

#endif

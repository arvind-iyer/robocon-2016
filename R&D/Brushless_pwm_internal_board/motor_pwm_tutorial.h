#ifndef _MOTOR_PWM_H_
#define _MOTOR_PWM_H_

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"	  
#include "stm32f10x_tim.h"

typedef struct {
	uint16_t motor_tim_ch;
	uint16_t motor_mag_pin;
	uint16_t motor_dir_pin;
	FunctionalState state;
	void (* oc_init_function)(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
} MOTOR_PWM_STRUCT[];

#define MOTOR0	0x00
#define MOTOR1	0x01
#define MOTOR2	0x02
#define MOTOR3	0x03
#define MOTOR_TIM	TIM3
#define MOTOR_TIM_RCC	RCC_APB1Periph_TIM3
#define MOTOR_MAG_PORT	GPIOC
#define MOTOR_DIR_PORT	GPIOG
#define MOTOR_MAG_GPIO_RCC	RCC_APB2Periph_GPIOC
#define MOTOR_DIR_GPIO_RCC	RCC_APB2Periph_GPIOG		


void motor_init(void);
//void motor_control( u8 motor_id , s16 val );   // value from -1000 to 1000

#endif		/*  _MOTOR_PWM_H_ */

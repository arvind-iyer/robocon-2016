#ifndef _SERVO_H_
#define _SERVO_H_

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"	  
#include "stm32f10x_tim.h"

#define SERVO_COUNT     4
#define SERVO_TIM				TIM8
#define SERVO_PORT			GPIOC
#define SERVO_TIM_RCC		RCC_APB2Periph_TIM8
#define SERVO_GPIO_RCC	RCC_APB2Periph_GPIOC

#define SERVO_MIN 700
#define SERVO_MED 1550
#define SERVO_MAX 2400

typedef struct {
	u16 servo_tim_ch;
	u16 servo_pin;
	FunctionalState state;
	void (*oc_init_function)(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
  void (*TIM_SetCompare) (TIM_TypeDef* TIMx, uint16_t Compare1);
} SERVO_PWM_STRUCT[];

typedef enum {
  SERVO1,
  SERVO2,
  SERVO3,
  SERVO4
} SERVO_ID;



void servo_init(void);
void servo_control(SERVO_ID servo_id , u16 val); 
void servo_control_all(u16 val); 

#endif		/*  _SERVO_H_ */

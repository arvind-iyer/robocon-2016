#ifndef _SERVO_H_
#define _SERVO_H_

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"	  
#include "stm32f4xx_tim.h"

#define SERVO_COUNT     4
#define SERVO_TIM				TIM3
#define SERVO_PORT			GPIOA
#define SERVO_TIM_RCC		RCC_APB1Periph_TIM3
#define SERVO_GPIO_RCC	RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB

typedef struct {
	u16 servo_tim_ch;
	GPIO_TypeDef* GPIOx;
	u16 servo_pin;
	uint16_t GPIO_PinSource;
	FunctionalState state;
	void (*oc_init_function)(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
  void (*TIM_SetCompare) (TIM_TypeDef* TIMx, uint32_t Compare1);
} SERVO_PWM_STRUCT[];

typedef enum {
  SERVO1, //0
  SERVO2, //1
  SERVO3, //2
  SERVO4 //3
} SERVO_ID;



void servo_init(void);
void servo_control(SERVO_ID servo_id , u16 val);   // value from 0 to 1000

#endif		/*  _SERVO_H_ */

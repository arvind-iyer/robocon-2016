/*
 * encoder.c
 *
 *  Created on: 2014-9-20ÈÕ
 *  Author: ThinkPad
 */
#include "encoder.h"

void encoder_init(){

	gpio_init();
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM1 , ENABLE);
	timer_init();

}

void gpio_init(){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	/* Enable Timer Clock Source */
	RCC_APB2PeriphClockCmd(ENCODER_TIMER_CLOCK_SOURCE_1, ENABLE);
 	/* Enable GPIO, clock */
	RCC_APB2PeriphClockCmd(ENCODER_TIMER_GPIO_CLOCK_SOURCE_1|RCC_APB2Periph_AFIO, ENABLE);
   	/* Set Timer Pin */
  	GPIO_InitStructure.GPIO_Pin = ENCODER_TIMER_PORT1_1 | ENCODER_TIMER_PORT2_1;
  	/* Set Pin Mode */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 	/* Activate */
  	GPIO_Init(ENCODER_TIMER_GPIOx_1, &GPIO_InitStructure);
	
		/* Enable Timer Clock Source */
	RCC_APB2PeriphClockCmd(ENCODER_TIMER_CLOCK_SOURCE_2, ENABLE);
 	/* Enable GPIO, clock */
	RCC_APB2PeriphClockCmd(ENCODER_TIMER_GPIO_CLOCK_SOURCE_2|RCC_APB2Periph_AFIO, ENABLE);
   	/* Set Timer Pin */
  	GPIO_InitStructure.GPIO_Pin = ENCODER_TIMER_PORT1_2 | ENCODER_TIMER_PORT2_2;
  	/* Set Pin Mode */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 	/* Activate */
  	GPIO_Init(ENCODER_TIMER_GPIOx_2, &GPIO_InitStructure);
}

void timer_init(){
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	/* clear */
	TIM_DeInit(ENCODER_TIMER_1);
	/* Set Start */
	TIM_TimeBaseStructure.TIM_Prescaler = 0x00;  // No prescaling
	/* Set Max Count */
	TIM_TimeBaseStructure.TIM_Period = 0xffff;
	/* Set Clock Devision */
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	/* Set Count Mode ----- ascending ar descending */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	/* Activate */
	TIM_TimeBaseInit(ENCODER_TIMER_1, &TIM_TimeBaseStructure);

	TIM_EncoderInterfaceConfig(ENCODER_TIMER_1, TIM_EncoderMode_TI12,
	                         TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	/* Reset counter */
	ENCODER_TIMER_1->CNT = 0;
	TIM_Cmd(ENCODER_TIMER_1, ENABLE);
	
		/* clear */
	TIM_DeInit(ENCODER_TIMER_1);
	/* Set Start */
	TIM_TimeBaseStructure.TIM_Prescaler = 0x00;  // No prescaling
	/* Set Max Count */
	TIM_TimeBaseStructure.TIM_Period = 0xffff;
	/* Set Clock Devision */
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	/* Set Count Mode ----- ascending ar descending */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	/* Activate */
	TIM_TimeBaseInit(ENCODER_TIMER_1, &TIM_TimeBaseStructure);

	TIM_EncoderInterfaceConfig(ENCODER_TIMER_1, TIM_EncoderMode_TI12,
	                         TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	/* Reset counter */
	ENCODER_TIMER_1->CNT = 0;
	TIM_Cmd(ENCODER_TIMER_1, ENABLE);
}

u32 getCount(){
	return ENCODER_TIMER_1->CNT;
}

u32 getDistance(u32 prevCount){
	return 0;
}


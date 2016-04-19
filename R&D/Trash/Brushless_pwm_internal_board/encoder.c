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
	RCC_APB2PeriphClockCmd(ENCODER_TIMER_CLOCK_SOURCE, ENABLE);
 	/* Enable GPIO, clock */
	RCC_APB2PeriphClockCmd(ENCODER_TIMER_GPIO_CLOCK_SOURCE|RCC_APB2Periph_AFIO, ENABLE);
   	/* Set Timer Pin */
  	GPIO_InitStructure.GPIO_Pin = ENCODER_TIMER_PORT1 | ENCODER_TIMER_PORT2;
  	/* Set Pin Mode */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 	/* Activate */
  	GPIO_Init(ENCODER_TIMER_GPIOx, &GPIO_InitStructure);
}

void timer_init(){
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	/* clear */
	TIM_DeInit(ENCODER_TIMER);
	/* Set Start */
	TIM_TimeBaseStructure.TIM_Prescaler = 0x00;  // No prescaling
	/* Set Max Count */
	TIM_TimeBaseStructure.TIM_Period = 0xffff;
	/* Set Clock Devision */
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	/* Set Count Mode ----- ascending ar descending */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	/* Activate */
	TIM_TimeBaseInit(ENCODER_TIMER, &TIM_TimeBaseStructure);

	TIM_EncoderInterfaceConfig(ENCODER_TIMER, TIM_EncoderMode_TI12,
	                         TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	/* Reset counter */
	ENCODER_TIMER->CNT = 0;
	TIM_Cmd(ENCODER_TIMER, ENABLE);
}

u32 getCount(){
	return ENCODER_TIMER->CNT;
}

u32 getDistance(u32 prevCount){
	u32 currCount = ENCODER_TIMER->CNT;
	static u32 distance = 0;
	if (prevCount - currCount > ENCODER_MAX_CHANGE){
		distance +=  0xffff;
	}
	else if (prevCount - currCount < -ENCODER_MAX_CHANGE){
		distance -=  0xffff;
	}
	distance += (currCount-prevCount);
	prevCount = currCount;
	return distance;
}


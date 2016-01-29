#include "ticks.h"

volatile u16 main_ticks = 0;
volatile u16 seconds = 0;

u16 get_ticks(){
	return main_ticks;
}

u16 get_seconds(){
	return seconds;
}

void ticks_init(void){
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
	TIM3->ARR = 100;
	TIM3->PSC = 72 - 1;
	TIM3->EGR = 1;
	TIM3->SR = 0;
	TIM3->DIER = 1;
	TIM3->CR1 = 1;
	/*
	  TIM_TimeBaseInitStructure.TIM_Prescaler;
  TIM_TimeBaseInitStructure.TIM_CounterMode;
  TIM_TimeBaseInitStructure.TIM_Period;
  TIM_TimeBaseInitStructure.TIM_ClockDivision;
	*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;   
	NVIC_Init(&NVIC_InitStructure);
}

void TIM3_IRQHandler(void){
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	main_ticks ++;
	if( main_ticks >= 10000 ){
		main_ticks = 0;
		seconds ++;
	}	
}

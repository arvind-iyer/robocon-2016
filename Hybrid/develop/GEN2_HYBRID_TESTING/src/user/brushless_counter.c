#include "brushless_counter.h"

void brushless_counter_init(){
	gpio_init(BRUSHLESS_COUNTER_GPIO, GPIO_Speed_50MHz, GPIO_Mode_AF_PP, 1);
	GPIO_PinRemapConfig(BRUSHLESS_COUNTER_TIM_REMAP, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF; //Max TIM period
	TIM_TimeBaseStructure.TIM_Prescaler = 35; // divide by 36
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(BRUSHLESS_COUNTER_TIM, &TIM_TimeBaseStructure);
	
	TIM_Cmd(BRUSHLESS_COUNTER_TIM, ENABLE); 
}

s32 get_brushless_counter(){
	return TIM_GetCounter(BRUSHLESS_COUNTER_TIM);
}

#include "ticks.h"

volatile u16 ticks = 0;
volatile u16 seconds = 0;

/**
  * @brief  Get the ticks passed from 0-999
  * @param  None
  * @retval ticks passed
  */
u16 get_ticks() {
	return ticks;
}

/**
  * @brief  Get the seconds passed from
  * @param  seconds
  * @retval ticks passed
  */
u16 get_seconds() {
	return seconds;
}

/**
  * @brief  Initialization of ticks timer
  * @param  None
  * @retval None
  */
void ticks_init(void) {
	
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6 , ENABLE);
	TIM6->ARR = 1000;
	TIM6->PSC = SystemCoreClock / 1000000 - 1;
	TIM6->EGR = 1;
	TIM6->SR = 0;
	TIM6->DIER = 1;
	TIM6->CR1 = 1;
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Timer for ticks
  * @param  None
  * @retval None
  */
void TIM6_IRQHandler(void) {
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

	if (ticks >= 999) {
		ticks = 0;
		seconds++;
	} else {
		ticks++;
	}
}
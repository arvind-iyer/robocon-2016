/**RPM Sensor for STM32F103
** @Author James Mok
**/

#include "RPM_sensor.h"

static volatile u32 past_val = 65535;
static volatile u32 now_val = 0;
static s32 diff = 0;

void RPMs_init(void){
	encoder_init();
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	
	TIM_DeInit(RPMs_Count_TIM);
	TIM_TimeBaseStructure.TIM_Period = 30000;	                 				       // Timer period, 30ms to trigger the event
	TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / 1000000 - 1;     // 72M/1M - 1 = 71
	TIM_TimeBaseInit(RPMs_Count_TIM, &TIM_TimeBaseStructure);      							 // this part feeds the parameter we set above
	
	TIM_ClearITPendingBit(RPMs_Count_TIM, TIM_IT_Update);												 // Clear Interrupt bits
	TIM_ITConfig(RPMs_Count_TIM, TIM_IT_Update, ENABLE);													 // Enable TIM Interrupt
	TIM_Cmd(RPMs_Count_TIM, ENABLE);																							 // Counter Enable

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_Init(&NVIC_InitStructure);	
}

u32 get_pulse(void){
	return get_count(RPMs_encoder);
}

/**
``* @brief 	return the RPS. return -1 if in wrong direction
``*	@Param 	None
	*	@retval RPS
	*/
u32 get_RPS(void){
	s32 buffer = diff;
	return buffer>0?buffer*1000 /(countPerRo * 30):buffer;
}

void TIM6_IRQHandler(void){
	if (TIM_GetITStatus(RPMs_Count_TIM, TIM_IT_Update) != RESET) {
    TIM_ClearFlag(RPMs_Count_TIM, TIM_FLAG_Update);
		now_val = get_pulse();
		if(now_val <= past_val)
		{
			diff = past_val - now_val;		//as the counter is down counting
		}
		else
		{
			diff = past_val + (65535 - now_val);
		}
		
		if(diff > overFlow)
			diff = -1;
		
		past_val = now_val;
	}
}
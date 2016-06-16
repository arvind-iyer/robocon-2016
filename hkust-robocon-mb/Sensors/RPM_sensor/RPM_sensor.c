/**RPM Sensor for STM32F103
** @Author James Mok
**/

#include "RPM_sensor.h"

u16 RPM = 0;

void RPMs_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO | RCC_APB2Periph_TIM1, ENABLE);
	
	GPIO_StructInit(&GPIO_InitStructure);																														// Set to default
	GPIO_InitStructure.GPIO_Pin = RPMs_Pin;						// Set Timer Pin 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;																						// Set Pin mode to floating
	GPIO_Init(RPMs_Port, &GPIO_InitStructure);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE); 

	TIM_DeInit(RPMs_in_TIM);																													// clear
	TIM_TimeBaseStructure.TIM_Prescaler = 0x00; 																										// No prescaling
	TIM_TimeBaseStructure.TIM_Period = 0xffff;																											// Max Count
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(RPMs_in_TIM, &TIM_TimeBaseStructure);
	
	TIM_TIxExternalClockConfig(RPMs_in_TIM, TIM_TIxExternalCLK1Source_TI2, TIM_ICPolarity_Rising, 0);
   
  TIM_Cmd(RPMs_in_TIM, ENABLE);
	
	TIM_DeInit(RPMs_Count_TIM);
	TIM_TimeBaseStructure.TIM_Period = 1000;	                 				       // Timer period, 1000 ticks in one second
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

u16 get_RPM(void){
	return RPM;
}

void TIM6_IRQHandler(void){
	if (TIM_GetITStatus(RPMs_Count_TIM, TIM_IT_Update) != RESET) {
    TIM_ClearFlag(RPMs_Count_TIM, TIM_FLAG_Update);
		RPM = TIM_GetCounter(RPMs_in_TIM) * 1000 / 6;
		TIM_SetCounter(RPMs_in_TIM, 0);
	}
}
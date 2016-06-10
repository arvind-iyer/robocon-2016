#include "encoder.h"

s32 encoder_cycles[2] = {0};
s32 encoder_cur_cycle[2] = {0};

//Wheel diameter: 90/2, same rmp as gear
//Gear:encoder ratio: 90:18
//Resol: 1024

static Encoder_Typedef encoder = {{ENCODER_TIMER1,ENCODER_TIMER1_CLOCK_SOURCE,ENCODER_TIMER1_GPIO_CLOCK_SOURCE,
																	 ENCODER_TIMER1_PORT1,ENCODER_TIMER1_PORT2,ENCODER_TIMER1_GPIOx},
																	{ENCODER_TIMER2,ENCODER_TIMER2_CLOCK_SOURCE,ENCODER_TIMER2_GPIO_CLOCK_SOURCE,
																	 ENCODER_TIMER2_PORT1,ENCODER_TIMER2_PORT2,ENCODER_TIMER2_GPIOx}};

/**
  * @brief  Initialization of encoder
  * @param  None
  * @retval None
  */
void encoder_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	//Alternate function enable: Encoder 1
	RCC_APB2PeriphClockCmd(ENCODER_TIMER1_CLOCK_SOURCE , ENABLE);
	RCC_AHB1PeriphClockCmd(ENCODER_TIMER1_GPIO_CLOCK_SOURCE, ENABLE);
	// Enable Timer Clock Source
	GPIO_PinAFConfig(ENCODER_TIMER1_GPIOx, ENCODER_TIMER1_GPIO_PINSOURCE1, ENCODER_TIMER1_AF);
	GPIO_PinAFConfig(ENCODER_TIMER1_GPIOx, ENCODER_TIMER1_GPIO_PINSOURCE2, ENCODER_TIMER1_AF);

	//Alternate function enable: Encoder 2
	RCC_APB1PeriphClockCmd(ENCODER_TIMER2_CLOCK_SOURCE , ENABLE);
	RCC_AHB1PeriphClockCmd(ENCODER_TIMER2_GPIO_CLOCK_SOURCE , ENABLE);
	
	// Enable Timer Clock Source
	GPIO_PinAFConfig(ENCODER_TIMER2_GPIOx, ENCODER_TIMER2_GPIO_PINSOURCE1, ENCODER_TIMER2_AF);
	GPIO_PinAFConfig(ENCODER_TIMER2_GPIOx, ENCODER_TIMER2_GPIO_PINSOURCE2, ENCODER_TIMER2_AF);
	
	// GPIO init Encoder 1
	GPIO_StructInit(&GPIO_InitStructure);																				
	GPIO_InitStructure.GPIO_Pin = ENCODER_TIMER1_PORT1 | ENCODER_TIMER1_PORT2;						
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(ENCODER_TIMER1_GPIOx, &GPIO_InitStructure);	

	//GPIO Init Encoder 2
	GPIO_StructInit(&GPIO_InitStructure);																	
	GPIO_InitStructure.GPIO_Pin = ENCODER_TIMER2_PORT1 | ENCODER_TIMER2_PORT2;						
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(ENCODER_TIMER2_GPIOx, &GPIO_InitStructure);

	// Timer init Encoder 1
	TIM_ClearITPendingBit(ENCODER_TIMER1, TIM_IT_Update);			
	TIM_DeInit(ENCODER_TIMER1);													
	TIM_TimeBaseStructure.TIM_Prescaler = 0x00; 		
	TIM_TimeBaseStructure.TIM_Period = 0xffff;	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(ENCODER_TIMER1, &TIM_TimeBaseStructure);

	//Timer init Encoder 2
	TIM_ClearITPendingBit(ENCODER_TIMER2, TIM_IT_Update);												
	TIM_DeInit(ENCODER_TIMER2);																							
	TIM_TimeBaseStructure.TIM_Prescaler = 0x00; 
	TIM_TimeBaseStructure.TIM_Period = 0xffff;								
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(ENCODER_TIMER2, &TIM_TimeBaseStructure);

	// Setting to Rising edge mode
	// Use TIM_EncoderMode_TI1 to count only B pin reading
	// TI2 to count only A pin
	// TI12 = TI3 to count both

	//Enable counting for Encoder 1
	TIM_EncoderInterfaceConfig(ENCODER_TIMER1, TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_SetCounter(ENCODER_TIMER1, 0);
	TIM_Cmd(ENCODER_TIMER1, ENABLE);	

	// Enable counting for Encoder 2
	TIM_EncoderInterfaceConfig(ENCODER_TIMER2, TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_SetCounter(ENCODER_TIMER2, 0);
	TIM_Cmd(ENCODER_TIMER2, ENABLE);
	
	reset_all_encoder();
}

//Return and update the encoder reading, need to call frequently, at least within a encoder cycle
s32 get_count(ENCODER ENCODERx){
	//if overflow occurs
	s32 diff = (s32)encoder_cur_cycle[ENCODERx] - (s32)TIM_GetCounter(encoder[ENCODERx].timer);
	if (diff > 30000){
		encoder_cycles[ENCODERx]++;
	}else if(diff < -30000){
		encoder_cycles[ENCODERx]--;
	}
	encoder_cur_cycle[ENCODERx] = TIM_GetCounter(encoder[ENCODERx].timer);
	return encoder_cycles[ENCODERx]*65535 + encoder_cur_cycle[ENCODERx];
}

s32 get_dis(ENCODER ENCODERx){
	return get_count(ENCODERx)*ENCODER_RATIO;
}

s32 get_average_dis(){
	return get_average_encoder()*ENCODER_RATIO;
}

s32 get_average_encoder(){
	return (get_count(ENCODER1) + get_count(ENCODER2)) / 2;
}

void reset_all_encoder(){
	for (u8 i=0;i<2;i++){
		TIM_SetCounter(encoder[i].timer, 0);
		encoder_cycles[i] = 0;
		encoder_cur_cycle[i] = 0;
	}
}


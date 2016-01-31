#include "encoder.h"

static Encoder_Typedef encoder = {{ENCODER_TIMER1,ENCODER_TIMER1_CLOCK_SOURCE,ENCODER_TIMER1_GPIO_CLOCK_SOURCE,
																	 ENCODER_TIMER1_PORT1,ENCODER_TIMER1_PORT2,ENCODER_TIMER1_GPIOx},
																	{ENCODER_TIMER2,ENCODER_TIMER2_CLOCK_SOURCE,ENCODER_TIMER2_GPIO_CLOCK_SOURCE,
																	 ENCODER_TIMER2_PORT1,ENCODER_TIMER2_PORT2,ENCODER_TIMER2_GPIOx}};

volatile u16 encoder_count[2][2] = {0};
u32 last_ticks = 0;
u16 encoder_vel[2] = {0};

/**
  * @brief  Initialization of encoder
  * @param  None
  * @retval None
  */
void encoder_init(void){
	u8 encoder_id;

//	TIM_ICInitTypeDef TIM_ICInitStructure;
	
	for (encoder_id = 0; encoder_id < ENCODER_NO; ++encoder_id) {
		GPIO_InitTypeDef GPIO_InitStructure;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		
		// Clock source enable
		RCC_APB1PeriphClockCmd(encoder[encoder_id].clock_source, ENABLE);																// Enable Timer Clock Source
		RCC_APB2PeriphClockCmd(encoder[encoder_id].GPIO_clock_source | RCC_APB2Periph_AFIO, ENABLE);		// Enable GPIO, clock
		
		// GPIO init
		GPIO_StructInit(&GPIO_InitStructure);																														// Set to default
		GPIO_InitStructure.GPIO_Pin = encoder[encoder_id].port1 | encoder[encoder_id].port2;	
		//Enable port 2 only
		//GPIO_InitStructure.GPIO_Pin = encoder[encoder_id].port2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;																						// Set Pin mode to floating
		GPIO_Init(encoder[encoder_id].GPIOx, &GPIO_InitStructure);																			// Activate
		if (encoder_id == 0) {
			GPIO_PinRemapConfig(GPIO_Remap_TIM4 , ENABLE);
		}
		
		// Timer init
		TIM_DeInit(encoder[encoder_id].timer);																										// clear
		TIM_TimeBaseStructure.TIM_Prescaler = 0x00; 																							// No prescaling
		TIM_TimeBaseStructure.TIM_Period = 0xffff;																								// Max Count
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(encoder[encoder_id].timer, &TIM_TimeBaseStructure);
		
		// Setting to Rising edge mode
				
		// Use TIM_EncoderMode_TI2 to count only B pin reading
		// TI1 to count only A pin
		// TI12 = TI3 to count both
		TIM_EncoderInterfaceConfig(encoder[encoder_id].timer, TIM_EncoderMode_TI1,
														 TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
		
		TIM_SetCounter(encoder[encoder_id].timer, 65535);																							// Reset Count as 0
		TIM_Cmd(encoder[encoder_id].timer, ENABLE);																									// Enable counter timer
		last_ticks = get_full_ticks();
		encoder_count[0][1] = 0;
		encoder_count[1][1] = 0;
	}
}

/**
  * @brief  Get the count reading from encoder.
	* @param  ENCODERx: where x can be 0 to 1
  * @retval The reading of the encoder
  */
u16 get_count(ENCODER ENCODERx){
	return encoder_count[ENCODERx][0];
}

void encoder_update(){
	u32 this_ticks = get_full_ticks();
	if (this_ticks==last_ticks) return;
	encoder_count[0][0] = 65535 - TIM_GetCounter(encoder[ENCODER1].timer);
	encoder_count[1][0] = 65535 - TIM_GetCounter(encoder[ENCODER2].timer);
	
	if (this_ticks-last_ticks>200){
		encoder_vel[0] = (encoder_count[0][0] - encoder_count[0][1])*100/(this_ticks - last_ticks);
		encoder_vel[1] = (encoder_count[1][0] - encoder_count[1][1])*100/(this_ticks - last_ticks);
		last_ticks = this_ticks;
		encoder_count[0][1] = encoder_count[0][0];
		encoder_count[1][1] = encoder_count[1][0];
	}
}

u16 get_vel(ENCODER ENCODERx){
	return encoder_vel[ENCODERx];
}


/**
* Reset all encoder readings to zero
**/
void encoder_reset_reading(){
	for (u8 encoder_id=0; encoder_id<ENCODER_NO; encoder_id++){
		TIM_SetCounter(encoder[encoder_id].timer, 0);
		encoder_vel[encoder_id] = 0;
		last_ticks = get_full_ticks();
		encoder_count[encoder_id][1] = 0;
		encoder_count[encoder_id][0] = 0;
		TIM_Cmd(encoder[encoder_id].timer, ENABLE);			
	}
}
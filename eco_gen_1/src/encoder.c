#include "encoder.h"

static Encoder_Typedef encoder = {{ENCODER_TIMER1,ENCODER_TIMER1_CLOCK_SOURCE,ENCODER_TIMER1_GPIO_CLOCK_SOURCE,
																	 ENCODER_TIMER1_PORT1,ENCODER_TIMER1_PORT2,ENCODER_TIMER1_GPIOx},
																	{ENCODER_TIMER2,ENCODER_TIMER2_CLOCK_SOURCE,ENCODER_TIMER2_GPIO_CLOCK_SOURCE,
																	 ENCODER_TIMER2_PORT1,ENCODER_TIMER2_PORT2,ENCODER_TIMER2_GPIOx}};

/**
  * @brief  Initialization of encoder
  * @param  Nonea
  * @retval None
  */
void encoder_init(void){
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    //Alternate function enable: Encoder 1
    RCC_APB1PeriphClockCmd(ENCODER_TIMER1_CLOCK_SOURCE , ENABLE);
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
    GPIO_StructInit(&GPIO_InitStructure);																														// Set to default
    GPIO_InitStructure.GPIO_Pin = ENCODER_TIMER1_PORT1 | ENCODER_TIMER1_PORT2;						
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(ENCODER_TIMER1_GPIOx, &GPIO_InitStructure);	

    //GPIO Init Encoder 2
    GPIO_StructInit(&GPIO_InitStructure);																														// Set to default
    GPIO_InitStructure.GPIO_Pin = ENCODER_TIMER2_PORT1 | ENCODER_TIMER2_PORT2;						
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(ENCODER_TIMER2_GPIOx, &GPIO_InitStructure);

    // Timer init Encoder 1
    TIM_ClearITPendingBit(ENCODER_TIMER1, TIM_IT_Update);												 // Clear Interrupt bits
    TIM_DeInit(ENCODER_TIMER1);																													// clear
    TIM_TimeBaseStructure.TIM_Prescaler = 0x00; 																										// No prescaling
    TIM_TimeBaseStructure.TIM_Period = 0xffff;																											// Max Count
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(ENCODER_TIMER1, &TIM_TimeBaseStructure);

    //Timer init Encoder 2
    TIM_ClearITPendingBit(ENCODER_TIMER2, TIM_IT_Update);												 // Clear Interrupt bits
    TIM_DeInit(ENCODER_TIMER2);																													// clear
    TIM_TimeBaseStructure.TIM_Prescaler = 0x00; 																										// No prescaling
    TIM_TimeBaseStructure.TIM_Period = 0xffff;																											// Max Count
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(ENCODER_TIMER2, &TIM_TimeBaseStructure);

    // Setting to Rising edge mode
    // Use TIM_EncoderMode_TI1 to count only B pin reading
    // TI2 to count only A pin
    // TI12 = TI3 to count both

    //Enable counting for Encoder 1
    TIM_EncoderInterfaceConfig(ENCODER_TIMER1, TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    //TIM_SetCounter(ENCODER_TIMER1, 0);																													// Reset Count as 0
    TIM_Cmd(ENCODER_TIMER1, ENABLE);	

    // Enable counting for Encoder 2
    TIM_EncoderInterfaceConfig(ENCODER_TIMER2, TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    //TIM_SetCounter(ENCODER_TIMER2, 0);																													// Reset Count as 0
    TIM_Cmd(ENCODER_TIMER2, ENABLE);
}

/**
  * @brief  Get the count reading from encoder.
	* @param  ENCODERx: where x can be 1 to 2, its value to be 0 or 1
  * @retval The reading of the encoder
  */
u32 get_count(ENCODER ENCODERx){
	return TIM_GetCounter(encoder[ENCODERx].timer);
}

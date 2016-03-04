#include "ultra_sonic.h"

static u16 last_sample_second = (u16) -1;
static u8 last_sample_count = 0;
static u8 sample_count = 0;

static US_TypeDef us_devices[US_COUNT];
static US_PROC_TypeDef us_proc[US_COUNT];


static US_MODE us_mode = US_INDEPENDENT;

static u8 current_us = 0;		// For US_TAKE_TURN
//static u16 us_take_turn_break = 0;

void us_init()
{
	TM_GPIO_Init(US_PORT, TRIG_PIN_1, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Fast);
	TM_GPIO_Init(US_PORT, ECHO_PIN_1, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_DOWN, TM_GPIO_Speed_Fast);
	
	for(u8 i=0;i<US_COUNT;i++)
	{
		us_devices[i].trigger_time_us = 0;
		us_devices[i].falling_time_us = 0;
		us_devices[i].pulse_width_tmp = 0;
		us_devices[i].pulse_width = 0;
		us_devices[i].state = US_NULL;
	}
	
	GPIO_WriteBit(US_PORT, TRIG_PIN_1, 0);
	
	last_sample_count = 0;
	last_sample_second = (u16) -1; 
	sample_count = 0;
	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;      									// TimeBase is for timer setting   > refer to P. 344 of library
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  EXTI_InitTypeDef   EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(US_RCC , ENABLE);
	GPIO_PinAFConfig(US_PORT, ECHO_PIN_1, GPIO_AF_TIM10);
	
	TIM_TimeBaseStructure.TIM_Prescaler = CLK / 1000000 - 1;     // 72M/1M - 1 = 71
	TIM_TimeBaseStructure.TIM_Period = US_RESET_TIME;	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(US_TIM, &TIM_TimeBaseStructure);      							 // this part feeds the parameter we set above
	
	TIM_ClearITPendingBit(US_TIM, TIM_IT_Update);												 // Clear Interrupt bits
	TIM_ITConfig(US_TIM, TIM_IT_Update, ENABLE);													 // Enable TIM Interrupt
  TIM_ITConfig(US_TIM, TIM_IT_CC1, ENABLE);													 // Enable TIM Interrupt
	TIM_Cmd(US_TIM, ENABLE);																							 // Counter Enable
  TIM_SetCounter(US_TIM, 0);
	
	
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       //set "high" to be effective output
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	              //produce output when counter < CCR
  TIM_OCInitStructure.TIM_Pulse = US_TRIG_PULSE;                                     // 10us
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
  TIM_OC1Init(US_TIM, &TIM_OCInitStructure);

  TIM_ARRPreloadConfig(US_TIM, ENABLE);

	
	// EXIT & NVIC Init
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource6);
  //GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource1);
  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_Init(&NVIC_InitStructure);
//	
//  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource3);
//  EXTI_InitStructure.EXTI_Line = EXTI_Line3;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);
//	
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
//	NVIC_Init(&NVIC_InitStructure);
//	
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource5);
//  EXTI_InitStructure.EXTI_Line = EXTI_Line5;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);

//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource7);
//  EXTI_InitStructure.EXTI_Line = EXTI_Line7;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);

//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource9);
//  EXTI_InitStructure.EXTI_Line = EXTI_Line9;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);
	
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = US_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}

static void us_trigger(u8 i)
{
	us_devices[i].trigger_time_us = 0;
	us_devices[i].falling_time_us = 0;
	GPIO_WriteBit(US_PORT, i, 1); // Trigger	
}


US_MODE us_get_mode(void)
{
	return us_mode;
}

US_STATE us_get_state(u8 i)
{
	if (i >= US_DEVICE_COUNT) {return US_READY;}
	return us_devices[i].state;
}

u32 us_get_pulse_raw(u8 i)
{
	if (i >= US_DEVICE_COUNT) {return 0;}
  return us_devices[i].pulse_width_tmp;
}

u32 us_get_pulse(u8 i)
{
	if (i >= US_DEVICE_COUNT) {return 0;}
   return us_devices[i].pulse_width; 
}

u32 us_get_distance(u8 i)
{
	if (i >= US_DEVICE_COUNT) {return 0;}
  return us_devices[i].pulse_width * 34 / 100 / 2;
}

u8 us_get_speed(void)
{
	if (last_sample_second != get_ticks()*1000) {
		return 0;
	}
	return last_sample_count; 
}

u8 us_get_current_us(void)
{
	return current_us;
}

void us_echo_interrupt(u8 i, u8 signal, u16 counter)
{
	if (signal) {
		us_devices[i].trigger_time_us = counter;
	} else {
		us_devices[i].falling_time_us = counter;
		if (us_devices[i].trigger_time_us == 0) {
			us_devices[i].pulse_width = 0;
		} else if (us_devices[i].trigger_time_us < us_devices[i].falling_time_us) {
			us_devices[i].pulse_width = us_devices[i].falling_time_us - us_devices[i].trigger_time_us;
		}
		//us_can_tx(i, us_get_distance(i));
	}
}

u8 us_in_range(u8 i) 
{
	return us_get_distance(i) > 20 && us_get_distance(i) <= 1000;
}

void us_proc_init(void)
{
	for (u8 i = 0; i < US_DEVICE_COUNT; ++i) {
		us_proc[i].in_range_full_ticks = 0;
		us_proc[i].out_range_full_ticks = 0;
		us_proc[i].in_distance = 0;
		us_proc[i].in_range_time = 0;
	}
}

void us_proc_update(void)
{
	for (u8 i = 0; i < US_DEVICE_COUNT; ++i) {
		if (us_proc[i].in_range_full_ticks == 0 && us_in_range(i)) {
			// Trigger
			us_proc[i].in_range_full_ticks = get_ticks();
			us_proc[i].in_distance = us_get_distance(i);
			LED_ON(LED_2);
		} else if (us_proc[i].in_range_full_ticks != 0 && !us_in_range(i)) {
			// Falling
			us_proc[i].out_range_full_ticks = get_ticks(); 
			us_proc[i].in_range_time = us_proc[i].out_range_full_ticks - us_proc[i].in_range_full_ticks;
			
			us_proc[i].in_range_full_ticks = 0;
			
			//us_can_proc_tx(i, us_proc[i].in_range_time, us_proc[i].in_distance);
		}
	}
}

void EXTI1_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line1) != RESET) {
		EXTI_ClearITPendingBit(EXTI_Line1);
		us_echo_interrupt(0, GPIO_ReadInputDataBit(US_PORT, ECHO_PIN_1), TIM_GetCounter(US_TIM));
  }
}

//void EXTI3_IRQHandler(void)
//{
//  if(EXTI_GetITStatus(EXTI_Line3) != RESET) {
//		EXTI_ClearITPendingBit(EXTI_Line3);
//		us_echo_interrupt(1, gpio_read_input(us_devices[1].echo_gpio), TIM_GetCounter(US_TIM));
//  }
//}

//void EXTI9_5_IRQHandler(void)
//{
//  if(EXTI_GetITStatus(EXTI_Line5) != RESET) {
//		EXTI_ClearITPendingBit(EXTI_Line5);
//		us_echo_interrupt(2, gpio_read_input(us_devices[2].echo_gpio), TIM_GetCounter(US_TIM));
//  }
//  if(EXTI_GetITStatus(EXTI_Line7) != RESET) {
//		EXTI_ClearITPendingBit(EXTI_Line7);
//		us_echo_interrupt(3, gpio_read_input(us_devices[3].echo_gpio), TIM_GetCounter(US_TIM));
//  }
//  if(EXTI_GetITStatus(EXTI_Line9) != RESET) {
//		EXTI_ClearITPendingBit(EXTI_Line9);
//		us_echo_interrupt(4, gpio_read_input(us_devices[4].echo_gpio), TIM_GetCounter(US_TIM));
//  }
//}


US_IRQHandler
{
  if (TIM_GetITStatus(US_TIM, TIM_IT_Update) != RESET) {   
		// RESET
		TIM_ClearITPendingBit(US_TIM, TIM_IT_Update);
		for (u8 i = 0; i < US_COUNT; ++i) {
			//us_can_tx(i, us_get_distance(i));
			LED_ON(LED_3);
			us_trigger(TRIG_PIN_1);
		}
    if (last_sample_second != get_ticks()/1000) {
				last_sample_second = get_ticks()/1000; 
				last_sample_count = sample_count; 
				LED_ON(LED_4);
				sample_count = 0;
		}
		++sample_count; 
  }
  
  
  if (TIM_GetITStatus(US_TIM, TIM_IT_CC1) != RESET) { 
		TIM_ClearITPendingBit(US_TIM, TIM_IT_CC1);
		for (u8 i = 0; i < US_COUNT; ++i) {
			us_devices[i].trigger_time_us = 0;
			us_devices[i].falling_time_us = 0;
			GPIO_WriteBit(US_PORT, TRIG_PIN_1, 0); // Trigger
		}
    
  } 
}

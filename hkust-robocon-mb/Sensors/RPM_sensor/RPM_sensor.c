/**RPM Sensor for STM32F103
** @Author James Mok
**/

#include "RPM_sensor.h"

static volatile u16 past_val = 0;
static volatile u16 now_val = 0;
static volatile u16 diff = 0;
static u16 tar_val = 0;
static u8 PID_FLAG = PID_ON;
s32 PID;

void RPMs_init(void){	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6 | RCC_APB1Periph_TIM5, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_StructInit(&GPIO_InitStructure);																														// Set to default
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;						// Set Timer Pin 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;																						// Set Pin mode to floating
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Init for RPMs_In_Tim
	TIM_DeInit(RPMs_In_TIM);																													// clear
	TIM_TimeBaseStructure.TIM_Prescaler = 0x00; 																										// No prescaling
	TIM_TimeBaseStructure.TIM_Period = CountPerRo;																											// Max Count
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseInit(RPMs_In_TIM, &TIM_TimeBaseStructure);
	TIM_EncoderInterfaceConfig(RPMs_In_TIM, TIM_EncoderMode_TI12,
													 TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	
	TIM_SetCounter(RPMs_In_TIM, 6);																													// Reset Count as 6
	TIM_ClearITPendingBit(RPMs_In_TIM, TIM_IT_Update);
	TIM_ITConfig(RPMs_In_TIM, TIM_IT_Update, ENABLE);
	TIM_Cmd(RPMs_In_TIM, ENABLE);// Enable counter timer
	
	//Init for RPMs_Count_TIM
	TIM_DeInit(RPMs_Count_TIM);
	TIM_TimeBaseStructure.TIM_Period = 450;	                 				       // Timer period, 450ms to trigger the event
	TIM_TimeBaseStructure.TIM_Prescaler = RPMs_TIM_Period;     // 72M/1e5 - 1 = 710
	TIM_TimeBaseInit(RPMs_Count_TIM, &TIM_TimeBaseStructure);      							 // this part feeds the parameter we set above
	
	TIM_ClearITPendingBit(RPMs_Count_TIM, TIM_IT_Update);												 // Clear Interrupt bits
	TIM_ITConfig(RPMs_Count_TIM, TIM_IT_Update, ENABLE);													 // Enable TIM Interrupt
	TIM_Cmd(RPMs_Count_TIM, ENABLE);																							 // Counter Enable
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}

/**
``* @brief 	return the RPS
``*	@Param 	None
	*	@retval RPS
	*/
u32 get_RPS(void){
	return 1 * diffScale / diff;
}

u32 get_diff(void){
	return diff;
}

void set_tar_val(u16 tar){
	tar_val = tar;
}

/**
	* @breif 	PID for burshless, to be called in ticks handler
	*	@Param 	P I D constant
	*	@retval Nonw
 **/
void RPMs_update(u16 P, u16 I, u16 D){
	static s16 now_error = 0;
	static s16 past_error = 0;
	static u16 now_pwm = 0;
	
	if(PID_FLAG == PID_OFF){
		servo_control_all(450);
	}
	else{
		now_error = tar_val - diff;
	    
		PID = P*now_error + I*(past_error + now_error) + D*(past_error + now_error)/2;
	
		now_pwm = 450 + PID;
		past_error = now_error;
	
		now_pwm = now_pwm >= 900 ? 900 : now_pwm;
		now_pwm = now_pwm <= 450 ? 450 : now_pwm;
	
		servo_control_all(now_pwm);
	}
}

void TIM5_IRQHandler(void){
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET){
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);
		diff = 1 * diffScale/TIM_GetCounter(RPMs_Count_TIM);
		PID_FLAG = diff >= RPMs_UpLim ? PID_OFF : PID_ON;
		
		TIM_SetCounter(RPMs_Count_TIM, 0);
	}
}

void TIM6_IRQHandler(void){
	if(TIM_GetITStatus(RPMs_Count_TIM, TIM_IT_Update) != RESET){
		TIM_ClearFlag(RPMs_Count_TIM, TIM_FLAG_Update);
		//PID_FLAG = PID_OFF;
		diff = 0;
	}
}
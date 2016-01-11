#include "motor_second.h"

void motor_second_init(){

    GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO, ENABLE);	// Enable bus

	//DIRECTION:PA10  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;						// Push-Pull Output Alternate-function
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	// PWM:PA9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;						// Push-Pull Output Alternate-function
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	

	
	//-------------TimeBase Initialization-----------//
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;			// counter will count up (from 0 to FFFF)
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;					//timer clock = dead-time and sampling clock 	
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	//------------------------------//
	TIM_TimeBaseStructure.TIM_Prescaler = 71;												//clk=72M/(71+1)= Hz, interval=? 
	TIM_TimeBaseStructure.TIM_Period = 200;												//pulse cycle=x  xperiod for one up
	//------------------------------//366hz
	//40300for turn on  

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	//final freq= timer freq / prescaler / period
	
	// ------------OC Init Configuration------------//
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   		// set "high" to be effective output
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;   		// set "high" to be effective output
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	           		// produce output when counter < CCR
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;		// Reset OC Idle state
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;	// Reset OC NIdle state
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  	// this part enable the normal output
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // this part enable the Nstate
	//------------------------------//
	TIM_OCInitStructure.TIM_Pulse = 200;														// this part sets the initial CCR value that mean the pwm value
	//------------------------------//
	\
	// OC Init
	TIM_OC2Init(TIM1, &TIM_OCInitStructure); //Initialize PA9 CH2
	TIM_OC2PreloadConfig(TIM1, ENABLE);
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);	
	TIM_SetCounter(TIM1,0);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);

}


void motor_second_control(u8 dir,u16 magnitude){//dir can be 1 or 0  pwm can be 1 or 2
    if(dir==0){
		GPIO_SetBits(GPIOA,GPIO_Pin_10); //dir
        TIM_SetCompare2(TIM1,magnitude);//PWM
	}
    else{
		GPIO_ResetBits(GPIOA,GPIO_Pin_10);//dir
		TIM_SetCompare2(TIM1,magnitude);//PWM
	}
}

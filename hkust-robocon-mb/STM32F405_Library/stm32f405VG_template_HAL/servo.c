#include "servo.h"

									
void servo_init(void){
  GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);	// Enable bus

	
	// PWM:PB13 PB15
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;						// Push-Pull Output Alternate-function
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	

	
	//-------------TimeBase Initialization-----------//
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;			// counter will count up (from 0 to FFFF)
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;					//timer clock = dead-time and sampling clock 	
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	
	
	//------------------------------//
	int TODO_Prescaler = 35;
	int TODO_Period = 40000;
	TIM_TimeBaseStructure.TIM_Prescaler = TODO_Prescaler;												//Timer speed = 72e6/(Prescaler+1) Hz
	TIM_TimeBaseStructure.TIM_Period = TODO_Period;												//Counts per pulse, basically max value for SetCompare
	//------------------------------//50Hz
	
	/*
	** 72e6Hz / (35 + 1) = 2000000Hz
	** 2e6Hz  / (40000)  = 50Hz
	*/
	

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	// ------------OC Init Configuration------------//
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   		// set "high" to be effective output
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;   		// set "high" to be effective output
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	           		// produce output when counter < CCR
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;		// Reset OC Idle state
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;	// Reset OC NIdle state
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  	// this part enable the normal output
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // this part enable the Nstate
	//------------------------------//
	TIM_OCInitStructure.TIM_Pulse = 4000;														// this part sets the initial CCR value that mean the pwm value
	//------------------------------//
	
	// OC Init


	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM1, ENABLE);
	
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);	
	TIM_SetCounter(TIM1,0);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
	}



void servo_control(uint16_t val){
    TIM_SetCompare4(TIM1,val);
}

/***************************************************************************************************	
	for hitec 5945 & 7955 and futaba S3010, min pulse width=900us, max pulse width=2100us
	val is the percentage of possible turning angles for the servo from 0.1% to 100%
	equation: desired_pulse_width=1/tim_clk*ccr_val
	
	example: desired_pulse_width=1232us, ccr_val=1232*1u/(1/250K)=308
		p.s. 1u (10^-6) is for normalising the scale for val since it is in 10^-6 scale
	i.e. min ccr_val=225, max ccr_val=525
	
***************************************************************************************************/

// Function for changing the PWM duty cycle  ( use when you change speed )
//  You can put your motor control function here
//  remember to declare your function in the .h file


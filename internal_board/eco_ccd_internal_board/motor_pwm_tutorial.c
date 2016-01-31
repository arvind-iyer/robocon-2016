#include "motor_pwm_tutorial.h"


//   Set the parameter of the MOTOR module  ( you can check this in the schematics file given ) 
void motor_init(void){
	static MOTOR_PWM_STRUCT motor_pwm ={
		{TIM_Channel_1, GPIO_Pin_6, GPIO_Pin_2, ENABLE, TIM_OC1Init},
		{TIM_Channel_2, GPIO_Pin_7, GPIO_Pin_3, ENABLE, TIM_OC2Init},
		{TIM_Channel_3, GPIO_Pin_8, GPIO_Pin_4, ENABLE, TIM_OC3Init},
		{TIM_Channel_4, GPIO_Pin_9, GPIO_Pin_5, ENABLE, TIM_OC4Init}};
	
  GPIO_InitTypeDef  MOTOR_MAG_GPIO_InitStructure;   // this part defines the variable we use 
	GPIO_InitTypeDef  MOTOR_DIR_GPIO_InitStructure;         
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;      // TimeBase is for timer setting   > refer to P. 344 of library
	TIM_OCInitTypeDef  TIM_OCInitStructure;             // OC is for channel setting within a timer  > refer to P. 342 of library
	u8 motor_id;
	
	RCC_APB1PeriphClockCmd(MOTOR_TIM_RCC, ENABLE);
	RCC_APB2PeriphClockCmd(MOTOR_DIR_GPIO_RCC | MOTOR_MAG_GPIO_RCC | RCC_APB2Periph_AFIO, ENABLE);
	
  MOTOR_MAG_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	MOTOR_MAG_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	MOTOR_DIR_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	MOTOR_DIR_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);            // this part feeds the parameter we set above
	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   // counter will count up (from 0 to FFFF)
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;       //timer clock = dead-time and sampling clock 	
	
	//-----------------------------------//
	TIM_TimeBaseStructure.TIM_Prescaler= 71;                         //10kHz
	TIM_TimeBaseStructure.TIM_Period = 1000;	                       	//	valid ccr value = 0-100
	//-----------------------------------//
	
	TIM_TimeBaseInit(MOTOR_TIM, &TIM_TimeBaseStructure);       // this part feeds the parameter we set above
	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         //set "high" to be effective output
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	             //produce output when counter < CCR

	//-----------------------------------//
	TIM_OCInitStructure.TIM_Pulse = 0;                               // this part sets the initial CCR value
	//-----------------------------------//

	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    // this part enable the output
	
	TIM_ARRPreloadConfig(MOTOR_TIM, ENABLE);
	TIM_Cmd(MOTOR_TIM, ENABLE);	

	for(motor_id=0; motor_id<4; motor_id++){                                    
		if(motor_pwm[motor_id].state==DISABLE)
			continue;
		MOTOR_MAG_GPIO_InitStructure.GPIO_Pin |= motor_pwm[motor_id].motor_mag_pin;
		MOTOR_DIR_GPIO_InitStructure.GPIO_Pin |= motor_pwm[motor_id].motor_dir_pin;
		motor_pwm[motor_id].oc_init_function(MOTOR_TIM, &TIM_OCInitStructure);      // feeds the OCInit
	}										
   GPIO_Init(MOTOR_MAG_PORT, &MOTOR_MAG_GPIO_InitStructure);	
	GPIO_Init(MOTOR_DIR_PORT, &MOTOR_DIR_GPIO_InitStructure);	
}

// Function for changing the PWM duty cycle  ( use when you change speed )
//  You can put your motor control function here
//  remember to declare your function in the .h file
   


#include "led.h"
u32 (*led_on[16]) (GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

void LED_GPIO_Config(void){		
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = FD10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_ResetBits(GPIOA, FD10 );
	
	GPIO_InitStructure.GPIO_Pin = FD1 | FD2 | FD3 | FD4| FD5 | FD15 | LED_RED | LED_GREEN | LED_BLUE;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);		
	GPIO_ResetBits(GPIOB, FD1 | FD2 | FD3 | FD4| FD5 | FD15 | LED_RED | LED_GREEN | LED_BLUE);

	GPIO_InitStructure.GPIO_Pin = FD0| FD7| FD8 | FD9 | FD11 | FD12 | FD13 | FD14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);		
	GPIO_ResetBits(GPIOC, FD0| FD7| FD8 | FD9 | FD11 | FD12 | FD13 | FD14);
	
	GPIO_InitStructure.GPIO_Pin = FD6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);	
	GPIO_ResetBits(GPIOD, FD6);
}

void LED_Control(u8 R, u8 G, u8 B){
//	if(R)		GPIO_SetBits(GPIOB, GPIO_Pin_10);
//	else		GPIO_ResetBits(GPIOB, GPIO_Pin_10);
//	if(G)		GPIO_SetBits(GPIOB, GPIO_Pin_12);
//	else		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
//	if(B)		GPIO_SetBits(GPIOB, GPIO_Pin_11);
//	else		GPIO_ResetBits(GPIOB, GPIO_Pin_11);
	if(R)		GPIO_SetBits(GPIOB, LED_RED);
	else		GPIO_ResetBits(GPIOB, LED_RED);
	if(G)		GPIO_SetBits(GPIOB, LED_GREEN);
	else		GPIO_ResetBits(GPIOB, LED_GREEN);
	if(B)		GPIO_SetBits(GPIOB, LED_BLUE);
	else		GPIO_ResetBits(GPIOB, LED_BLUE);
}

void IndicatorControl(u8 id){
	GPIO_ResetBits(GPIOA, FD10);
	GPIO_ResetBits(GPIOB, FD1 | FD2 | FD3 | FD4| FD5 | FD15);
	GPIO_ResetBits(GPIOC, FD0| FD7| FD8 | FD9 | FD11 | FD12 | FD13 | FD14);
	GPIO_ResetBits(GPIOD, FD6);
	/*
	if(id == 0)				{GPIO_SetBits(GPIOB, GPIO_Pin_9);}
	else if(id == 1)	{GPIO_SetBits(GPIOB, GPIO_Pin_8);}
	else if(id == 2)	{GPIO_SetBits(GPIOB, GPIO_Pin_7);}
	else if(id == 3)	{GPIO_SetBits(GPIOB, GPIO_Pin_6);}
	else if(id == 4)	{GPIO_SetBits(GPIOB, GPIO_Pin_5);}
	
	else if(id == 5)	{GPIO_SetBits(GPIOC, GPIO_Pin_12);}
	else if(id == 6)	{GPIO_SetBits(GPIOC, GPIO_Pin_11);}
	else if(id == 7)	{GPIO_SetBits(GPIOC, GPIO_Pin_10);}
	
	else if(id == 8)	{GPIO_SetBits(GPIOA, GPIO_Pin_8);}
	
	else if(id == 9)	{GPIO_SetBits(GPIOC, GPIO_Pin_9);}
	else if(id == 10)	{GPIO_SetBits(GPIOC, GPIO_Pin_8);}
	else if(id == 11)	{GPIO_SetBits(GPIOC, GPIO_Pin_7);}
	else if(id == 12)	{GPIO_SetBits(GPIOC, GPIO_Pin_6);}
	
	else if(id == 13)	{GPIO_SetBits(GPIOB, GPIO_Pin_15);}
	else if(id == 14)	{GPIO_SetBits(GPIOB, GPIO_Pin_14);}
	else if(id == 15)	{GPIO_SetBits(GPIOB, GPIO_Pin_13);}
	*/
	
	if(id == 0)				{GPIO_SetBits(GPIOC, FD0);}
	else if(id == 1)	{GPIO_SetBits(GPIOC, FD0); GPIO_SetBits(GPIOB, FD1);}
	
	else if(id == 2)	{GPIO_SetBits(GPIOB, FD1);}
	else if(id == 3)	{GPIO_SetBits(GPIOB, FD1); GPIO_SetBits(GPIOB, FD2);}
	
	else if(id == 4)	{GPIO_SetBits(GPIOB, FD2);}
	else if(id == 5)	{GPIO_SetBits(GPIOB, FD2);GPIO_SetBits(GPIOB, FD3);}
	
	else if(id == 6)	{GPIO_SetBits(GPIOB, FD3);}
	else if(id == 7)	{GPIO_SetBits(GPIOB, FD3);GPIO_SetBits(GPIOB, FD4);}
	
	else if(id == 8)	{GPIO_SetBits(GPIOB, FD4);}
	else if(id == 9)	{GPIO_SetBits(GPIOB, FD4);GPIO_SetBits(GPIOB, FD5);}
	
	else if(id == 10)	{GPIO_SetBits(GPIOB, FD5);}
	else if(id == 11)	{GPIO_SetBits(GPIOB, FD5);GPIO_SetBits(GPIOD, FD6);}
	
	else if(id == 12)	{GPIO_SetBits(GPIOD, FD6);}
	else if(id == 13)	{GPIO_SetBits(GPIOD, FD6);GPIO_SetBits(GPIOC, FD7);}
	
	else if(id == 14)	{GPIO_SetBits(GPIOC, FD7);}
	else if(id == 15)	{GPIO_SetBits(GPIOC, FD7);GPIO_SetBits(GPIOC, FD8);}
	
	else if(id == 16)	{GPIO_SetBits(GPIOC, FD8);}
	else if(id == 17)	{GPIO_SetBits(GPIOC, FD8);GPIO_SetBits(GPIOC, FD9);}
	
	else if(id == 18)	{GPIO_SetBits(GPIOC, FD9);}
	else if(id == 19)	{GPIO_SetBits(GPIOC, FD9);GPIO_SetBits(GPIOA, FD10);}
	
	else if(id == 20)	{GPIO_SetBits(GPIOA, FD10);}
	else if(id == 21)	{GPIO_SetBits(GPIOA, FD10);GPIO_SetBits(GPIOC, FD11);}
	
	else if(id == 22)	{GPIO_SetBits(GPIOC, FD11);}
	else if(id == 23)	{GPIO_SetBits(GPIOC, FD11);GPIO_SetBits(GPIOC, FD12);}
	
	else if(id == 24)	{GPIO_SetBits(GPIOC, FD12);}
	else if(id == 25)	{GPIO_SetBits(GPIOC, FD12);GPIO_SetBits(GPIOC, FD13);}
	
	else if(id == 26)	{GPIO_SetBits(GPIOC, FD13);}
	else if(id == 27)	{GPIO_SetBits(GPIOC, FD13);GPIO_SetBits(GPIOC, FD14);}
	
	else if(id == 28)	{GPIO_SetBits(GPIOC, FD14);}
	else if(id == 29)	{GPIO_SetBits(GPIOC, FD14);GPIO_SetBits(GPIOB, FD15);}
	
	else if(id == 30)	{GPIO_SetBits(GPIOB, FD15);}
	
	
	else{
			GPIO_ResetBits(GPIOA, FD10);
			GPIO_ResetBits(GPIOB, FD1 | FD2 | FD3 | FD4| FD5 | FD15);
			GPIO_ResetBits(GPIOC, FD0| FD7| FD8 | FD9 | FD11 | FD12 | FD13 | FD14);
			GPIO_ResetBits(GPIOD, FD6);
		}
}
/*
void TIM_LED_Pwm_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	TIM5->ARR = 5-1;
	TIM5->PSC = SystemCoreClock / 1000000 - 1;	// 7199 -> 1000000 Hz
	TIM5->EGR = 1;
	TIM5->SR = 0;
	TIM5->DIER = 1;
	TIM5->CR1 = 1;
	TIM_Cmd(TIM1, ENABLE);		// To be enabled when "buzzer_control" function is called
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;   
	NVIC_Init(&NVIC_InitStructure);

}

void led_pwm_control(u8 led_id , s32 ccr_val) {	            // input the motor_id you change and the corresponding value
												                // 1 = 0.1%,  1000 = 100%     determine
    if(ccr_val < -1000)                          // this part limit your input for ccr_val 
			ccr_val = -999;
		else if(ccr_val > 1000)
			ccr_val = 999;
	
	if (ccr_val > 0){                                           // the +ve or -ve represent 2 direction of rotation
		if(motor_id == MOTOR1_PWM )
			GPIO_SetBits( GPIOA, GPIO_Pin_5 );
		else
			GPIO_SetBits(GPIOA, GPIO_Pin_4 );
	}
	else {
		if(motor_id == MOTOR1_PWM )
			GPIO_ResetBits(GPIOA, GPIO_Pin_5 );
		else
			GPIO_ResetBits(GPIOA, GPIO_Pin_4 );
		ccr_val = -ccr_val;
	}

  if (motor_id == MOTOR1_PWM )                                      // this part feed your ccr value into the register
		//TIM_SetCompare3(TIM5, ccr_val );      				  // Compare1 is for channel 1 
		pwm_setbit[0] = ccr_val/2;												// Halved for 24V setting
	else
		//TIM_SetCompare4(TIM5, ccr_val );					  // Compare2 is for channel 2 
		pwm_setbit[1] = ccr_val/2;
}

void TIM5_IRQHandler(void){
	TIM_ClearFlag(TIM5, TIM_FLAG_Update);
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	pwm_counter++;
	if(pwm_counter > 1000){
		pwm_counter = 0;
	}
	if(pwm_counter < pwm_setbit[0]){
		GPIO_SetBits(GPIOA, GPIO_Pin_2 );
	}
	else{
		GPIO_ResetBits(GPIOA, GPIO_Pin_2 );
	}
	if(pwm_counter < pwm_setbit[1]){
		GPIO_SetBits(GPIOA, GPIO_Pin_3 );
	}
	else{
		GPIO_ResetBits(GPIOA, GPIO_Pin_3 );
	}
}
*/

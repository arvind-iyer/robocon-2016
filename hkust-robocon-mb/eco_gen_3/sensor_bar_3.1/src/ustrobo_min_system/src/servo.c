#include "servo.h"

static SERVO_PWM_STRUCT servo_pwm = {{TIM_Channel_1, GPIO_Pin_6, ENABLE, TIM_OC1Init, TIM_SetCompare1},
																		{TIM_Channel_2, GPIO_Pin_7, ENABLE, TIM_OC2Init, TIM_SetCompare2},
																		{TIM_Channel_3, GPIO_Pin_8, ENABLE, TIM_OC3Init, TIM_SetCompare3},
																		{TIM_Channel_4, GPIO_Pin_9, ENABLE, TIM_OC4Init, TIM_SetCompare4}};

/**
  * @brief  Servo initialization
  * @param  None
  * @retval None
  */
void servo_init(void){

	GPIO_InitTypeDef SERVO_GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	u8 servo_id;

	RCC_APB2PeriphClockCmd(SERVO_TIM_RCC, ENABLE);
	RCC_APB2PeriphClockCmd(SERVO_GPIO_RCC | RCC_APB2Periph_AFIO, ENABLE);	// Enable bus

	SERVO_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;						// Push-Pull Output Alternate-function
	SERVO_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		
	//-------------TimeBase Initialization-----------//
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;			// counter will count up (from 0 to FFFF)
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;					//timer clock = dead-time and sampling clock 	
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	//------------------------------//
	TIM_TimeBaseStructure.TIM_Prescaler = 71;												//clk=72M/(71+1)= Hz, interval=?
	TIM_TimeBaseStructure.TIM_Period = 20000;												//pulse cycle= 20000
	//------------------------------//

	TIM_TimeBaseInit(SERVO_TIM, &TIM_TimeBaseStructure);
	
	
	// ------------OC Init Configuration------------//
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   		// set "high" to be effective output
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;   		// set "high" to be effective output
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	           		// produce output when counter < CCR
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;		// Reset OC Idle state
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;	// Reset OC NIdle state
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  	// this part enable the output
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputState_Disable; // this part disable the Nstate
	//------------------------------//
	TIM_OCInitStructure.TIM_Pulse = 6667;														// this part sets the initial CCR value
	//------------------------------//
	
	// OC Init
	TIM_OC1Init(SERVO_TIM, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(SERVO_TIM, ENABLE);

	TIM_OC2Init(SERVO_TIM, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(SERVO_TIM, ENABLE);
	
	TIM_OC3Init(SERVO_TIM, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(SERVO_TIM, ENABLE);
	
	TIM_OC4Init(SERVO_TIM, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(SERVO_TIM, ENABLE);
	
	TIM_ARRPreloadConfig(SERVO_TIM, ENABLE);
	TIM_Cmd(SERVO_TIM, ENABLE);	
	TIM_CtrlPWMOutputs(SERVO_TIM, ENABLE);

	for (servo_id = 0; servo_id < 4; ++servo_id){
		if (servo_pwm[servo_id].state == DISABLE) {
			continue;
		}
		SERVO_GPIO_InitStructure.GPIO_Pin|=servo_pwm[servo_id].servo_pin;
//		servo_pwm[servo_id].oc_init_function(SERVO_TIM, &TIM_OCInitStructure);
	}
	GPIO_Init(SERVO_PORT, &SERVO_GPIO_InitStructure);	
}

/**
  * @brief  Controlling servos
  * @param  servo_id: Port of Servo to be used (SERVO1, SERVO2, SERVO3, SERVO4)
  * @param  val:Value from @SERVO_MIN to @SERVO_MAX
  */

void servo_control(SERVO_ID servo_id , u16 val) {
	 
	u16 ccr_val = val;
	val = (val<SERVO_MIN ? SERVO_MIN : (val>SERVO_MAX ? SERVO_MAX : val));
	
  if (((u8) servo_id) < SERVO_COUNT) {
    servo_pwm[servo_id].TIM_SetCompare(SERVO_TIM, ccr_val);
  }
}

/**
  * @brief  Controlling all servos
  * @param  val:Value from @SERVO_MIN to @SERVO_MAX
  */

void servo_control_all(u16 val) {
	 
	u16 ccr_val = val;
	val = (val<SERVO_MIN ? SERVO_MIN : (val>SERVO_MAX ? SERVO_MAX : val));
	
  for (u8 i=0; i<SERVO_COUNT; i++) {
    servo_pwm[i].TIM_SetCompare(SERVO_TIM, ccr_val);
  }
}
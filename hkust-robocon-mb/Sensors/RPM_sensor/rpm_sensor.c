#include "rpm_sensor.h"


u32 RPMS_vel = 0;		//Counter value always positive



/**
	*	@brief	Initializes RPM sensor and count, hardware usage defined in "rpm_sensor.h"
	* @param	
	* @rtval	
	**/
void RPMS_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOE|RCC_APB2Periph_TIM1,ENABLE);

	RPMS_vel = 0;
	
	//GPIO Init and Remap
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = RPMS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM1,ENABLE);
	
  /* TIM1 Input trigger configuration: External Trigger connected to TI2 */
	TIM_ICInitTypeDef TIM_ICInitStructure;
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_ICInit(RPMS_CNT_TIMx, &TIM_ICInitStructure);

	//Tim for Counter Init
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(RPMS_CNT_TIMx, &TIM_TimeBaseStructure);	

	// Sensor as the External Clock  Source
  TIM_TIxExternalClockConfig(RPMS_CNT_TIMx, TIM_TIxExternalCLK1Source_TI2, TIM_ICPolarity_Rising, 0x0);
	//TIM_ETRClockMode1Config(RPMS_CNT_TIMx, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0x00);
	TIM_SelectSlaveMode(RPMS_CNT_TIMx, TIM_SlaveMode_External1);
	
	TIM_CtrlPWMOutputs(RPMS_CNT_TIMx, ENABLE);
	TIM_Cmd(RPMS_CNT_TIMx, ENABLE);
}


/**
	*	@brief	Updates RPM sensor count to velocity buffer to be called every ticks
	* @param	
	* @rtval	
	**/
void RPMS_update(void)
{
	RPMS_vel = TIM_GetCounter(RPMS_CNT_TIMx);
	TIM_SetCounter(RPMS_CNT_TIMx, 0);
}

/**
	*	@brief	Returns RPM sensor velocity
	* @param	
	* @rtval	Velocity detected from RPM sensor per update
	**/
u32 RPMS_getVel(void)
{
	return RPMS_vel;
}




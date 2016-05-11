/**
*	Laser Sensor for stm32f103
*	@Desciption This is the library for the SICK DT50 laser sensor, which mesaures the distance from 200mm to 5000mm with 3.5% error from the board
*	@Reminder 	A 220nF capacitor must be presented at the adc port in order to filter out the noise
*	@Author 		James Mok
*
*	@Update			2016/5/1	Fix the error problem, which greatly increase accuracy, support 2 laser sensors(without DMA)
**/

//ADC3 -> adc2 channel 5


#include "laser_sensor.h"

static u16 ls_adc_reading[2];
static u16 ls_cal_reading[2];

static u32 avg[ls_number][avg_length] = {	{0} };

/* var for the turning the sensors */
static const u16 min_adc[2] = {18, 18};
static const u16 max_adc[2] = {3036, 3036};
static const u16 min_dis[2] = {200, 200};
static const u16 max_dis[2] = {5000, 5000};


void ls_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	#if (ls_number == 2)
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //see data sheet p.29 to see which pin can be used
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
		
		ADC_DeInit(ADC1);
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_InitStructure.ADC_ScanConvMode = DISABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		ADC_InitStructure.ADC_NbrOfChannel = 1;
		ADC_Init(ADC1, &ADC_InitStructure);
		RCC_ADCCLKConfig(RCC_PCLK2_Div8);
		ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_1Cycles5);
		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //see data sheet p.29 to see which pin can be used
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOC,&GPIO_InitStructure);
		
		ADC_DeInit(ADC3);
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_InitStructure.ADC_ScanConvMode = DISABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		ADC_InitStructure.ADC_NbrOfChannel = 1;
		ADC_Init(ADC3, &ADC_InitStructure);
		RCC_ADCCLKConfig(RCC_PCLK2_Div8);
		ADC_RegularChannelConfig(ADC3, ADC_Channel_13, 1, ADC_SampleTime_1Cycles5);
	}
	#else
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //see data sheet p.29 to see which pin can be used
		GPIO_InitStructure.GPIO_Pin = ls_pin1 | ls_pin2;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(ls_port,&GPIO_InitStructure);

		ADC_DeInit(ADC1);
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_InitStructure.ADC_ScanConvMode = DISABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		ADC_InitStructure.ADC_NbrOfChannel = 1;
		ADC_Init(ADC1, &ADC_InitStructure);
		RCC_ADCCLKConfig(RCC_PCLK2_Div8);
		ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_1Cycles5);
		
		ADC_DeInit(ADC3);
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_InitStructure.ADC_ScanConvMode = DISABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		ADC_InitStructure.ADC_NbrOfChannel = 1;
		ADC_Init(ADC3, &ADC_InitStructure);
		RCC_ADCCLKConfig(RCC_PCLK2_Div8);
		ADC_RegularChannelConfig(ADC3, ADC_Channel_11, 1, ADC_SampleTime_1Cycles5);
	}
	#endif

	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);

	while(ADC_GetResetCalibrationStatus(ADC1)); //Initial calib
	ADC_StartCalibration(ADC1);	/* Start ADC1 calibaration */
	
	while(ADC_GetCalibrationStatus(ADC1));	/* Check the end of ADC1 calibration */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);  /* Start ADC1 Software Conversion */
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
	ls_adc_reading[0] = ADC_GetConversionValue(ADC1);
	
	if(ls_number>1)
	{
		ADC_Cmd(ADC3, ENABLE);
		ADC_ResetCalibration(ADC3);

		while(ADC_GetResetCalibrationStatus(ADC3)); //Initial calib
		ADC_StartCalibration(ADC3);	/* Start ADC1 calibaration */
		
		while(ADC_GetCalibrationStatus(ADC3));	/* Check the end of ADC1 calibration */
		ADC_SoftwareStartConvCmd(ADC3, ENABLE);
		while(!ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC));
		ls_adc_reading[1] = ADC_GetConversionValue(ADC3);
	}

	for(u8 i=0;i<ls_number;i++)
	{
		for(u8 j=0;j<avg_length;j++)
			avg[i][j] = ls_adc_reading[i];
	}
}
/**
*	@brief return the range found
* @return the range
**/

u32 get_ls_cal_reading(u8 device)
{
  // Start the conversion
	if(device==0)
	{
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
		ls_adc_reading[0] = ADC_GetConversionValue(ADC1);
	}
	else
	{
		ADC_SoftwareStartConvCmd(ADC3, ENABLE);
		while(!ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC));
		ls_adc_reading[1] = ADC_GetConversionValue(ADC3);
	}

	ls_avg(device);
	
	if(ls_adc_reading[device]>=min_adc[device])
	{
		ls_cal_reading[device] = (ls_adc_reading[device] - min_adc[device]) * (max_dis[device] - min_dis[device]) / (max_adc[device] - min_adc[device]) + min_dis[device];
		if(ls_cal_reading[device] >= max_dis[device])
			ls_cal_reading[device] = max_dis[device];
	}
	else
		ls_cal_reading[device] = 0;
	
	return ls_cal_reading[device];
}

u32 get_ls_adc_reading(u8 device)
{
	if(device==0)
	{
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
		ls_adc_reading[0] = ADC_GetConversionValue(ADC1);
	}
		else
	{
		ADC_SoftwareStartConvCmd(ADC3, ENABLE);
		while(!ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC));
		ls_adc_reading[1] = ADC_GetConversionValue(ADC3);
	}

	return ls_adc_reading[device];
}

/**
*	@brief averaging the raw input
*	@return NULL
**/

void ls_avg(u8 device)
{
	u32 temp;
	
	if(device==0)
	{
		for(u8 h=0;h<avg_length-1;h++)
			avg[device][h] = avg[device][h+1];
		avg[device][avg_length-1] = ls_adc_reading[0];
	
		for(u8 i=0;i<avg_length;i++)
			temp += avg[device][i];
		ls_adc_reading[device] = temp/avg_length;
	}
	else
	{
		for(u8 h=0;h<avg_length-1;h++)
			avg[device][h] = avg[device][h+1];
		avg[device][avg_length-1] = ls_adc_reading[1];
	
		for(u8 i=0;i<avg_length;i++)
			temp += avg[device][i];
		ls_adc_reading[device] = temp/avg_length;
	}
}

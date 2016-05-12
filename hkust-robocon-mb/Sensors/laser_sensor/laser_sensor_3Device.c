/**
*	Laser Sensor for stm32f103
*	@Desciption This is the library for the SICK DT50 laser sensor, which mesaures the distance from 200mm to 5000mm with 3.5% error from the board
*	@Reminder 	A 220nF capacitor must be presented at the adc port in order to filter out the noise
*	@Author 		James Mok
**/

#include "laser_sensor.h"

static u16 ls_adc_reading[ls_number];
static u16 ls_cal_reading[ls_number];
static u16 ls_dma_reading[ls_number];

static u32 avg[ls_number][avg_length];

static const u16 min_adc[ls_number] = {16, 16, 16};
static const u16 max_adc[ls_number] = {3037, 3037, 3037};
static const u16 min_dis[ls_number] = {200, 200, 200};
static const u16 max_dis[ls_number] = {2500, 2500, 2500};

u16* ls_dma_reading_ptr = &ls_dma_reading[0];

void ls_init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);
	
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = ls_pin1 | ls_pin2 | ls_pin3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //see data sheet p.29 to see which pin can be used
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ls_port,&GPIO_InitStructure);
	
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u16)&ls_dma_reading[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 8;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1,ENABLE);

	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 3;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_1Cycles5);
	
	ADC_Cmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1)); //Initial calib
	ADC_StartCalibration(ADC1);	/* Start ADC1 calibaration */
	while(ADC_GetCalibrationStatus(ADC1));	/* Check the end of ADC1 calibration */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);  /* Start ADC1 Software Conversion */
	
	
	
//	while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
//	DMA_ClearFlag(DMA1_FLAG_TC1);
//	for(u8 i=0;i<ls_number;i++)
//	{
//		for(u8 j=0;j<avg_length;j++)
//			avg[i][j] = ls_dma_reading[i];
//	}
}
/**
*	@brief return the range found
* @return the range
**/

u32 get_ls_cal_reading(u8 device)
{
  // Start the conversion
//  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
//	ADC_Cmd(ADC1, ENABLE);
//	while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
//	DMA_ClearFlag(DMA1_FLAG_TC1);
	//ls_avg(device);
	
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
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
//	ADC_Cmd(ADC1, ENABLE);
//	while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
//	DMA_ClearFlag(DMA1_FLAG_TC1);
	//ls_avg(device);
	ls_adc_reading[device] = ls_dma_reading[device];
	return ls_adc_reading[device];
}

/**
*	@brief averaging the raw input
*	@return NULL
**/

void ls_avg(u8 device)
{
	u32 temp;
	
	for(u8 h=0;h<avg_length-1;h++)
		avg[device][h] = avg[device][h+1];
	avg[device][avg_length-1] = ls_dma_reading[device];
	
	for(u8 i=0;i<avg_length;i++)
		temp += avg[device][i];
	ls_adc_reading[device] = temp/avg_length;
}

u16 ls_cal_reading_one(void)
{
	return ls_cal_reading[0];
}
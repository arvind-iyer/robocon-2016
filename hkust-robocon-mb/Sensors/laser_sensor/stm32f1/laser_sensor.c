/**
*	Laser Sensor for stm32f103
*	@desciption This is the library for the SICK DT50 laser sensor, which mesaures the distance from 200mm to 5000mm with 3.5% error from the board
*	
*	@Author James Mok
**/

#include "laser_sensor.h"

uint32_t ls_reading;
uint32_t ls_reading1;
u8 ls_test = 0;
u8 indx = 0;
u8 ok = 0;

u32 avg[avg_length] = {0};

void ls_init()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHBPeriphClockCmd(ls_rcc, ENABLE);
	
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //see data sheet p.29 to see which pin can be used
	GPIO_InitStructure.GPIO_Pin = ls_pin;
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
	
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ls_reading;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 16;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1,ENABLE);
	
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);

	while(ADC_GetResetCalibrationStatus(ADC1)); //Initial calib
	ADC_StartCalibration(ADC1);	/* Start ADC1 calibaration */
	
	while(ADC_GetCalibrationStatus(ADC1));	/* Check the end of ADC1 calibration */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);  /* Start ADC1 Software Conversion */
}
/**
*	@brief return the range found
* @return the range
**/

u32 get_ls_reading()
{
  // Start the conversion
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	if(!ok)
		ls_avg_init();
	ls_avg();
	
	
	if(ls_reading>=min_adc)
		ls_reading1 = (ls_reading - min_adc) * max_dis / max_adc + min_dis;
	else
		ls_reading1 = 0;
	
	return ls_reading;
}

/**
*	@brief init for the averaging method
*	@return NULL
**/

void ls_avg_init()
{
	avg[indx] = ls_reading;
	indx++;
	if(indx==avg_length)
	{
		indx = 0;
		ok = 1;
	}
}

/**
*	@brief averaging the raw input
*	@return NULL
**/

void ls_avg()
{
	u32 temp;
	
	for(u8 h=0;h<avg_length-1;h++)
		avg[h] = avg[h+1];
	avg[avg_length-1] = ls_reading;
	
	for(u8 i=0;i<avg_length;i++)
		temp += avg[i];
	ls_reading = temp/avg_length;
}
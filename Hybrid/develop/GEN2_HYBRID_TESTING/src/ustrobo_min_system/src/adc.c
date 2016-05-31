#include "adc.h"
/**
*	ADC Library for STM32F103_4_2_mainboard
* @Author	James Mok, Simon Tam
*	
**/

/*You can comment any unwanted ADC for closeing that ADC port, 
	but the priority should be consistent and it affects the index of the data array.
*/
ADC_structure adc_struct[] = {{GPIOC, ADC_1_Pin, ADC_Channel_13, 1, ADC_SampleTime_239Cycles5}, 
															{GPIOC,	ADC_2_Pin, ADC_Channel_12, 2, ADC_SampleTime_239Cycles5},
															{GPIOC,	ADC_3_Pin, ADC_Channel_11, 3, ADC_SampleTime_239Cycles5},
															{GPIOA,	ADC_4_Pin, ADC_Channel_4, 4, ADC_SampleTime_239Cycles5},
															{GPIOA,	ADC_5_Pin, ADC_Channel_5, 5, ADC_SampleTime_239Cycles5},
															{GPIOA,	ADC_6_Pin, ADC_Channel_6, 6, ADC_SampleTime_239Cycles5},
															{GPIOA,	ADC_7_Pin, ADC_Channel_7, 7, ADC_SampleTime_239Cycles5},
															{GPIOC,	ADC_8_Pin, ADC_Channel_14, 8, ADC_SampleTime_239Cycles5},
															{GPIOC,	ADC_9_Pin, ADC_Channel_15, 9, ADC_SampleTime_239Cycles5}};

static u32 adc_value[ADC_Device];		//Data array that stores ADC result

void adc_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;	
	RCC_APB2PeriphClockCmd(ADC_GPIO_RCC, ENABLE);
	
	for(u8 i=0; i<ADC_Device; i++)
	{
		GPIO_InitStructure.GPIO_Pin = adc_struct[i].ADC_Pin;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(adc_struct[i].GPIO, &GPIO_InitStructure);
	}
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = ADC_Device;
	ADC_Init(ADC1, &ADC_InitStructure); 
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	for(u8 j=0; j<ADC_Device; j++)
	{
		ADC_RegularChannelConfig(ADC1, adc_struct[j].ADC_Channel, adc_struct[j].priority, adc_struct[j].sample_time);
	}
  
	DMA_InitTypeDef DMA_InitStructure;             
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;	//The address of ADC1 result register
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&adc_value[0];			//The address of the array that will store the data
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = ADC_Device;									//The size of the data array
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
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
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


u32 get_adc(u8 i)
{
	return adc_value[i];
}
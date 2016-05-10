#include "adc.h"

static u32  adc_value[17];

#define ADC_NUM_DR_ADDRESS     ((uint32_t)0x4001224C)

#define ADC_NUM_CHANNELS	2

static volatile uint16_t dataBuffer[ADC_NUM_CHANNELS];

//AO1: PC4/ADC12_IN14
//AO2: PC5/ADC12_IN15

void adc_init(void)
{
    
    	// Enable GPIO clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	// Pins as analog input
	GPIO_InitTypeDef      GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// Enable DMA clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	DMA_InitTypeDef       DMA_InitStructure;
	DMA_InitStructure.DMA_Channel = DMA_Channel_2;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC_NUM_DR_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)dataBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = ADC_NUM_CHANNELS;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	DMA_Cmd(DMA2_Stream0, ENABLE);


	// Enable ADC clock
	RCC_APB2PeriphClockCmd(ADC_PERIPH, ENABLE);

	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	ADC_InitTypeDef       ADC_InitStructure;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 2;
	ADC_Init(ADC_NUM, &ADC_InitStructure);


	ADC_RegularChannelConfig(ADC_NUM, ADC_CHANNEL_1, 1, ADC_SampleTime_3Cycles);
	ADC_RegularChannelConfig(ADC_NUM, ADC_CHANNEL_2, 2, ADC_SampleTime_3Cycles);


	ADC_DMARequestAfterLastTransferCmd(ADC_NUM, ENABLE);

	ADC_DMACmd(ADC_NUM, ENABLE);

    ADC_Cmd(ADC_NUM, ENABLE);
// DMA_InitTypeDef DMA_InitStructure;
//             
// RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
// DMA_DeInit(DMA1_Channel1);
// DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
// DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&adc_value[1];
// DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
// DMA_InitStructure.DMA_BufferSize = 16;
// DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
// DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
// DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
// DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
// DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
// DMA_InitStructure.DMA_Priority = DMA_Priority_High;
// DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
// DMA_Init(DMA1_Channel1, &DMA_InitStructure);
// DMA_Cmd(DMA1_Channel1,ENABLE);

// GPIO_InitTypeDef GPIO_InitStructure;
// ADC_InitTypeDef ADC_InitStructure;
// 
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
// 
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
// GPIO_Init(GPIOA, &GPIO_InitStructure);

// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
// 
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
// GPIO_Init(GPIOB, &GPIO_InitStructure);
// 
// RCC_APB2PeriphClockCmd(ADC_PERIPH, ENABLE);
//// ADC_DeInit(ADC1);
// ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC1和ADC2工作在独立模式
// ADC_InitStructure.ADC_ScanConvMode = ENABLE;//多通道
// ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//连续转换
// ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件启动转换
// ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//转换结果右对齐
// ADC_InitStructure.ADC_NbrOfChannel = 2;//通道数目
// ADC_Init(ADC1, &ADC_InitStructure); 
// 
// RCC_ADCCLKConfig(RCC_PCLK2_Div8); //PCLK 6分频
// ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5);//通道，转换次序，转换时间PA1
// ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_239Cycles5);//PB1
// 
// ADC_DMACmd(ADC1, ENABLE);
// 
// ADC_Cmd(ADC1, ENABLE);
// ADC_ResetCalibration(ADC1);
// while(ADC_GetResetCalibrationStatus(ADC1));
// ADC_StartCalibration(ADC1);//开始校准
// while(ADC_GetCalibrationStatus(ADC1));
// ADC_SoftwareStartConvCmd(ADC1, ENABLE);//使能ADC的软件转换启动功能
}


u32 get_adc(u8 i){
    return adc_value[i];
}

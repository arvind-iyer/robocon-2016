#include "adc.h"

static u32  adc_value[17];

void adc_init(void)
{
 DMA_InitTypeDef DMA_InitStructure;
             
 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
 DMA_DeInit(DMA1_Channel1);
 DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
 DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&adc_value[1];
 DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
 DMA_InitStructure.DMA_BufferSize = 16;
 DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不变
 DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存地址递增
 DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
 DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
 DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
 DMA_InitStructure.DMA_Priority = DMA_Priority_High;
 DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
 DMA_Init(DMA1_Channel1, &DMA_InitStructure);
 DMA_Cmd(DMA1_Channel1,ENABLE);

 GPIO_InitTypeDef GPIO_InitStructure;
 ADC_InitTypeDef ADC_InitStructure;
 
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
// 
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
// GPIO_Init(GPIOA, &GPIO_InitStructure);

RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
 GPIO_Init(GPIOB, &GPIO_InitStructure);
 
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
// ADC_DeInit(ADC1);
 ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC1和ADC2工作在独立模式
 ADC_InitStructure.ADC_ScanConvMode = ENABLE;//多通道
 ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//连续转换
 ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件启动转换
 ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//转换结果右对齐
 ADC_InitStructure.ADC_NbrOfChannel = 2;//通道数目
 ADC_Init(ADC1, &ADC_InitStructure); 
// ADC_TempSensorVrefintCmd(ENABLE);//使能片内温度传感器
 
 RCC_ADCCLKConfig(RCC_PCLK2_Div8); //PCLK 6分频
//ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5);//通道，转换次序，转换时间PA1
 ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_239Cycles5);//PB1
 //ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_55Cycles5);
 //ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_55Cycles5);
// ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 6, ADC_SampleTime_239Cycles5);
 
 ADC_DMACmd(ADC1, ENABLE);
 
 ADC_Cmd(ADC1, ENABLE);
 ADC_ResetCalibration(ADC1);
 while(ADC_GetResetCalibrationStatus(ADC1));
 ADC_StartCalibration(ADC1);//开始校准
 while(ADC_GetCalibrationStatus(ADC1));
 ADC_SoftwareStartConvCmd(ADC1, ENABLE);//使能ADC的软件转换启动功能
}


u32 get_adc(u8 i){

return adc_value[i];
}
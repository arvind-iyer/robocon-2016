#include "main.h"



volatile u16 ADC_val[16] = {0};

u8 sensor_pos = 0;
u8 LED_STATE;
u8 buttonA = 0, buttonB = 0;
extern u8 colour_now[];

extern struct Reading* hsv_ptr;
extern struct Reading* output_ptr;
extern struct Reading* colour_list_ptr;



void GPIO_init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//Buttons
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;       
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	//ADC
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void ADC_init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_InitTypeDef ADC_InitStructure;
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 16;
    ADC_Init(ADC1, &ADC_InitStructure);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 6, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 7, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 8, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 9, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 10, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 11, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 12, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 13, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 14, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 15, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 16, ADC_SampleTime_55Cycles5);
	
	
	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);

	while(ADC_GetResetCalibrationStatus(ADC1)); //Initial calib
	ADC_StartCalibration(ADC1);	/* Start ADC1 calibaration */
	
	while(ADC_GetCalibrationStatus(ADC1));	/* Check the end of ADC1 calibration */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);  /* Start ADC1 Software Conversion */
}

void DMA_init(void){
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_InitTypeDef  DMA_InitStructure;
	DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_val;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 16; //8
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	
	ADC_DMACmd(ADC1, ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

void dllm(CanRxMsg msg)
{
	if(msg.Data[0]==1)
	{
		GPIO_ResetBits(GPIOA, FD10);
		GPIO_ResetBits(GPIOB, FD1);
	}
}

void system_init(){
	GPIO_init();
	LED_init();
	ADC_init();
	DMA_init();
	ticks_init();
	can_init();
	can_rx_init();
	//readFlash();
	
	LED_STATE = 0;
	buttonA = 0;
	buttonB = 0;
	//can_rx_add_filter(0x032, CAN_RX_MASK_EXACT, dllm);
}

void flag_test(u8 flag)
{
	CAN_MESSAGE msg;
	msg.id = 0x0C5;
	msg.length = 1;
	msg.data[0] = flag;
	can_tx_enqueue(msg);
}

int main(void)
{
	system_init();
	u8 temp = 100;
	
	while(1)
	{
		//press both button A and B to init the colour list
		if((!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)) && (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15)))
		{
			while((!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)) || (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15)));
			colour_init();
			//writeFlash();
		}
		
		if((!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)) && (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15)))
		{
			while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) && (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15)));
			buttonA++;
			if(buttonA>2)
				buttonA = 0;
		}
		if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) && (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)))
		{
			while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) && (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)));
			buttonB++;
			if(buttonB>3)
				buttonB = 0;
		}
		
		set_colour(LED_STATE); //Set the LED color
		
        _delay_us(delay_time);
		ADC_Cmd(ADC1, ENABLE);	//start ADC convertion
		while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET)); //waiting for DMA is finished
		data_collect();
		
		LED_STATE++;
		if(LED_STATE>2)
			LED_STATE = 0;
		DMA_ClearFlag(DMA1_FLAG_TC1);
		
		//read_reading(output_ptr, buttonA, buttonB);
		//flag_test(temp);
		read_colour(); //Send the data
	}
}

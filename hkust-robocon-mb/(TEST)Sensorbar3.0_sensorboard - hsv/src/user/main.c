/**SensorBar3.0 program. Please read the READ_ME before use
*@Author James Mok, Andre Chan
**/

#include "main.h"


volatile u16 ADC_val[16] = {0};
struct Reading max_1;
struct Reading now;

u8 sat[16] = {0};
uint32_t startAddress = 0x8009000; //0x8019000;
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

void writeFlash(void){
	u8 i;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(startAddress);

	for(i = 0; i < 16; i++){
		FLASH_ProgramHalfWord((startAddress + (4*i)),max_1.off_reading[i]);
	}
    
    for(i = 0 ; i < 16; i++){
        FLASH_ProgramHalfWord((startAddress + 64 + (4 * i)),max_1.red_reading[i]);
    }
    
    for(i = 0 ; i < 16; i++){
        FLASH_ProgramHalfWord((startAddress + 128 + (4 * i)),max_1.green_reading[i]);
    }
    
    for(i = 0 ; i < 16; i++){
        FLASH_ProgramHalfWord((startAddress + 192 + (4 * i)),max_1.blue_reading[i]);
    }
	FLASH_Lock();
}

void readFlash(void){
  u8 i;
    for(i=0; i<16; i++){
		max_1.off_reading[i] = *(uint16_t *)(startAddress + (4 * i));
		
	}
    for(i = 0 ; i < 16; i++){
        max_1.red_reading[i] = *(uint16_t *)(startAddress + 64 + (4 * i));
    }
    for(i = 0 ; i < 16; i++){
        max_1.green_reading[i] = *(uint16_t *)(startAddress + 128 + (4 * i));
    }
	
    for(i=0; i<16; i++){
		max_1.blue_reading[i] = *(uint16_t *)(startAddress + 192 + (4 * i));
	}
}

/**********************************************************************************************/

#define DELAY_US 1000 //800
#define scalar 1000
//RGB to HSV converter
void rgb_hsv_converter(struct Reading* reading){
		//RGB to HSV
		u16 h_max[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		u16 h_min[16] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
		u8 index = 0;	
        
	for(u8 i=0; i < 16;i++)
	{
		if(h_max[i] < reading->red_reading[i])
		{
			h_max[i] = reading->red_reading[i];
			index = 1;
		}
		if(h_max[i] < reading->green_reading[i])
		{
			h_max[i] = reading->green_reading[i];
			index = 2;
		}
		if(h_max[i] < reading->blue_reading[i])
		{
			h_max[i] = reading->blue_reading[i];
			index = 3;
		}
		
		if(h_min[i] > reading->red_reading[i])
			h_min[i] = reading->red_reading[i];
		if(h_min[i] > reading->green_reading[i])
			h_min[i] = reading->green_reading[i];
		if(h_min[i] > reading->blue_reading[i])
			h_min[i] = reading->blue_reading[i];
		
        //Determining the Hue
		if((h_max[i] - h_min[i])==0)
			reading->h[i] = 0;
		
        else if(index == 1)
		{
			s16 temp = (((reading->green_reading[i] * scalar - reading->blue_reading[i]*scalar) / (h_max[i] - h_min[i])) % (6*scalar));
			temp = (temp<0)?temp+(6*scalar):temp;
			reading->h[i]=(60*temp)/scalar;
		}
        
		else if(index==2) reading->h[i]=(60*((reading->blue_reading[i]*scalar - reading->red_reading[i]*scalar)/(h_max[i] - h_min[i])+2*scalar))/scalar;
		else if(index==3) reading->h[i]=(60*((reading->red_reading[i]*scalar - reading->green_reading[i]*scalar)/(h_max[i] - h_min[i])+4*scalar))/scalar;
		
        //Determining the Saturation
        if (h_max[i]==0) reading->s[i] = 0;
        else reading->s[i] = ((h_max[i]-h_min[i])*100) / h_max[i];
		
        //Determining the value
        reading->v[i] = h_max[i] * 100/255;
	}
}	

//init 3 color LED
void RGB_init(){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//init button A,B
void button_init(){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
	


//init sensor by detecting white color
void sensor_init(struct Reading*  max){
	for(u8 i=0;i<16;i++)
	{
		max->off_reading[i] = 0;
		max->red_reading[i] = 0;
		max->green_reading[i] = 0;
		max->blue_reading[i] = 0;
	}
	
	
		while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14));
		printf("calibrate white\n");
		for (u8 i=0;i<40;i++){
			_delay_us(DELAY_US);
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
			for(u8 i=0;i<16;i++)max->off_reading[i] = ADC_val[i];
			DMA_ClearFlag(DMA1_FLAG_TC1);
			
            GPIO_SetBits(GPIOB,GPIO_Pin_12);
			_delay_us(DELAY_US);
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
			//if (max[1]< (ADC_GetConversionValue(ADC1)-max[0]))
				//max[1] = ADC_GetConversionValue(ADC1);
			for(u8 i=0;i<16;i++)
			{
				if(max->red_reading[i] < (ADC_val[i] - max->off_reading[i]))
					max->red_reading[i] = ADC_val[i];
			}
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);
            DMA_ClearFlag(DMA1_FLAG_TC1);

			GPIO_SetBits(GPIOB,GPIO_Pin_11);
			_delay_us(DELAY_US);
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
			//if (max[2]< (ADC_GetConversionValue(ADC1)-max[0]))
				//max[2] = ADC_GetConversionValue(ADC1);
			for(u8 i=0;i<16;i++)
			{
				if(max->green_reading[i] < (ADC_val[i] - max->off_reading[i]))
					max->green_reading[i] = ADC_val[i];
			}
			GPIO_ResetBits(GPIOB,GPIO_Pin_11);
            DMA_ClearFlag(DMA1_FLAG_TC1);
			
			GPIO_SetBits(GPIOB,GPIO_Pin_13);
			_delay_us(DELAY_US);
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
			//if (max[3]< (ADC_GetConversionValue(ADC1))-max[0])
				//max[3] = ADC_GetConversionValue(ADC1);
			for(u8 i=0;i<16;i++)
			{
				if(max->blue_reading[i] < (ADC_val[i] - max->off_reading[i]))
					max->blue_reading[i] = ADC_val[i];
			}
			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
            DMA_ClearFlag(DMA1_FLAG_TC1);
		}
        writeFlash();
        _delay_ms(500);
}

//init ADC channels
void ADC_init(void){
	ADC_InitTypeDef ADC_InitStructure;	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;       
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	 
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;       
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	 
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;       
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	 
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 16;
    ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10 ,1 ,ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11 ,2 ,ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12 ,3 ,ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13 ,4 ,ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0 ,5 ,ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1 ,6 ,ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2 ,7 ,ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3 ,8 ,ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4 ,9 ,ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5 ,10 ,ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6 ,11 ,ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7 ,12 ,ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14 ,13 ,ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15 ,14 ,ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8 ,15 ,ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9 ,16 ,ADC_SampleTime_1Cycles5);


	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1)); 
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1) == SET);
}

int main(void)
{
	//init button A,B
	button_init();
	//init 3 color LED
	RGB_init();	
	//init ADC
	ADC_init();
	//init uart
	uart_init(COM1, 256000);
    //init DMA
    DMA_init();
    
    //ticks init
    ticks_init();
    
    //can initialization
    can_init();
	can_rx_init();
    
    //Threshold
    int border;
    
    //Color count
    int color_count[16];
    
    //Hue average
    int hueAverage = 0;
	
	u8 counter=0;
    
    readFlash();
    _delay_ms(500);
	while (1)
	{
		for(u8 i=0;i<16;i++)
		{
			now.off_reading[i] = 0;
			now.red_reading[i] = 0;
			now.green_reading[i] = 0;
			now.blue_reading[i] = 0;
		}
		
		//callibrate white color
		if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14))
		{
			sensor_init(&max_1);
		}
		
		//discrete data collect
		_delay_us(DELAY_US);
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		
        //Collect off reading
        while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
		
        
        for(u8 i=0;i<16;i++) now.off_reading[i] = ADC_val[i];
        DMA_ClearFlag(DMA1_FLAG_TC1);

		//Collect Red
        GPIO_SetBits(GPIOB,GPIO_Pin_12); //Red
		_delay_us(DELAY_US);
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
		for(u8 i=0;i<16;i++)now.red_reading[i] = ADC_val[i];
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
        DMA_ClearFlag(DMA1_FLAG_TC1);

		//Collect Green
        GPIO_SetBits(GPIOB,GPIO_Pin_11); //Green
		_delay_us(DELAY_US);
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
		for(u8 i=0;i<16;i++)now.green_reading[i] = ADC_val[i];
		GPIO_ResetBits(GPIOB,GPIO_Pin_11);
        DMA_ClearFlag(DMA1_FLAG_TC1);
		
		//Collect Blue
        GPIO_SetBits(GPIOB,GPIO_Pin_13); //Blue
		_delay_us(DELAY_US);
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
		for(u8 i=0;i<16;i++)now.blue_reading[i] = ADC_val[i];
		GPIO_ResetBits(GPIOB,GPIO_Pin_13);
        DMA_ClearFlag(DMA1_FLAG_TC1);

		for(u8 i=0;i < 16;i++)
		{
			if(now.red_reading[i] < now.off_reading[i]) now.red_reading[i] = now.off_reading[i];
			if(now.green_reading[i] < now.off_reading[i]) now.green_reading[i] = now.off_reading[i];
			if(now.blue_reading[i] < now.off_reading[i]) now.blue_reading[i] = now.off_reading[i];
			if(now.red_reading[i] > max_1.red_reading[i]) now.red_reading[i] = max_1.red_reading[i];
			if(now.green_reading[i] > max_1.green_reading[i]) now.green_reading[i] = max_1.green_reading[i];
			if(now.blue_reading[i] > max_1.blue_reading[i]) now.blue_reading[i] = max_1.blue_reading[i];
			
			//normalizing RGB
			now.red_reading[i] = (now.red_reading[i] - now.off_reading[i])*255 / (max_1.red_reading[i] - now.off_reading[i]);
			now.green_reading[i] = (now.green_reading[i] - now.off_reading[i])*255 / (max_1.green_reading[i] - now.off_reading[i]);
			now.blue_reading[i] = (now.blue_reading[i] - now.off_reading[i])*255 / (max_1.blue_reading[i] - now.off_reading[i]);
		}
              		
		//hsv calling
		rgb_hsv_converter(&now);
        for(int i = 0 ; i < 16;i++){
            hueAverage  += now.h[i];
        }
        hueAverage /= 16;
        
    //New algo:
    //Dark blue
    if(hueAverage >= 201 && hueAverage <= 230)border = 45;
    
    //Orange
    else if(hueAverage <= 30)border = 50;
    
    //Light green
    else if(hueAverage >= 80 && hueAverage <= 100) border = 30;
    
    //Dark green
    else if(hueAverage >= 160 && hueAverage <= 200) border = 45;
    
    //Pink
    else if(hueAverage > 300) border = 45;
    
    else border = 45;
        
    for(int i=0;i < 16;i++)
    {
        if(now.s[i] < border) sat[i] = 1; //White color
        else sat[i] = 0; //Dark color
    }
    
	CAN_MESSAGE msg;
	msg.id = 0x0C5;
	msg.length = 8;
	for(int i=0;i<8;i++)msg.data[i] = sat[i];
	can_tx_enqueue(msg);
	
	CAN_MESSAGE msg1;
	msg1.id = 0x0C6;
	msg1.length = 8;
	for(int i=0;i<8;i++)msg1.data[i] = sat[i+8];
	can_tx_enqueue(msg1);
    //#define 	OFFSET		40/10
    //printf("N: %d\tR: %d\tG: %d\tB: %d\r\n", val[0]*256/4096*OFFSET, val[1]*256/4096*OFFSET, val[2]*256/4096*OFFSET, val[3]*256/3072*OFFSET);
//    if (get_full_ticks() % 200 == 0){
//        printf("Sensor:%d\n",0);
//        printf("N : %d \tR: %d\tG: %d\tB: %d\r\n",max_1.off_reading[0],max_1.red_reading[0], max_1.green_reading[0], max_1.blue_reading[0]);
//        printf("N: %d\tR: %d\tG: %d\tB: %d\r\n",now.off_reading[0], now.red_reading[0], now.green_reading[0], now.blue_reading[0]);
//        printf("[hsv] H(360): %d\tS(100): %d\tV(100): %d\r\n",now.h[13],now.s[13],now.v[13]);
//        printf("HueAvg: %d\n",hueAverage);
//        printf("\n");
//    }

    //Clear da flaggg
    
	}
}

/*******************************************************************************************/

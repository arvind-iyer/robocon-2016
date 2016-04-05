#include "lightSensor.h"

volatile extern u16 ADC_val[16];
Reading max_1;
Reading now;
u8 sat[16] = {0};

//Threshold
int border;
//Hue average
int hueAverage = 0; 

void initToZero(){
    for(int i = 0 ; i < 16 ; i++){
        now.off_reading[i] = 0;
        now.red_reading[i] = 0;
        now.green_reading[i] = 0;
        now.blue_reading[i] = 0;
    }
}

void sensor_init(Reading*  max){
	for(u8 i=0;i<16;i++)
	{
		max->off_reading[i] = 0;
		max->red_reading[i] = 0;
		max->green_reading[i] = 0;
		max->blue_reading[i] = 0;
	}
	
	
		while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14));
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

void dataCollect(){
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
}


//RGB to HSV converter
void rgb_hsv_converter(Reading* reading){
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

void sendData(){
    for(int i = 0 ; i < 16;i++){
            hueAverage  += now.h[i];
    }
    hueAverage /= 16;
    
    //Dark blue
    if(hueAverage >= 201 && hueAverage <= 250)border = 50;
    
    //Orange
    else if(hueAverage <= 30)border = 55;
    
    //Light green
    else if(hueAverage >= 80 && hueAverage <= 100) border = 35;
    
    //Dark green
    else if(hueAverage >= 160 && hueAverage <= 200) border = 42;
    
    //Pink
    else if(hueAverage > 300) border = 45;
    
    //Random Color
    else border = 45;
        
    for(int i=0;i < 16;i++)
    {
        if(now.s[i] >  (border + 5)) sat[i] = 0; //White color
        else sat[i] = 1; //Dark color
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
    
    CAN_MESSAGE msg2;
    msg2.id = 0x0C7;
    msg2.length = 3;
    msg2.data[1] = hueAverage;
    msg2.data[2] = border;
    if(hueAverage >= 170 && hueAverage <= 250)msg2.data[0] = 1;
    else msg2.data[0] = 0;
    can_tx_enqueue(msg2);
}


void printInformation(){
    if (get_full_ticks() % 200 == 0){
        printf("Sensor:%d\n",0);
        printf("N : %d \tR: %d\tG: %d\tB: %d\r\n",max_1.off_reading[0],max_1.red_reading[0], max_1.green_reading[0], max_1.blue_reading[0]);
        printf("N: %d\tR: %d\tG: %d\tB: %d\r\n",now.off_reading[0], now.red_reading[0], now.green_reading[0], now.blue_reading[0]);
        printf("[hsv] H(360): %d\tS(100): %d\tV(100): %d\r\n",now.h[13],now.s[13],now.v[13]);
        printf("HueAvg: %d\n",hueAverage);
        printf("\n");
    }
}

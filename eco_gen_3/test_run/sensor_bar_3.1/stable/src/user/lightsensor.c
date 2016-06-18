#include "lightSensor.h"

volatile extern u16 ADC_val[16];
extern u8 calibrationStage;
u16 calibratedHueAverage[NUMOFAREAS] = {0};
u16 calibratedSaturationAverage[NUMOFAREAS] = {0};
u16 calibratedValueAverage[NUMOFAREAS] = {0};
u8 currentZone;

Reading now;
Reading max_1;
Reading tempReading;

u8 sat[16] = {0};

bool first_scan = false;



//Hue average
u32 hueAverage = 0; 
int green = 0;
int avgS = 0;
int fullWhite = 0;

void initToZero(){
    for(int i = 0 ; i < 16 ; i++){
        now.off_reading[i] = 0;
        now.red_reading[i] = 0;
        now.green_reading[i] = 0;
        now.blue_reading[i] = 0;
    }
}

void sensor_init(Reading*  max){
    while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14));
    if(calibrationStage == 0){
        for(u8 i=0;i<16;i++)
        {
            max->off_reading[i] = 0;
            max->red_reading[i] = 0;
            max->green_reading[i] = 0;
            max->blue_reading[i] = 0;
        }
        _delay_us(DELAY_US);
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
        for(u8 i=0;i<16;i++)max->off_reading[i] = ADC_val[i];
        DMA_ClearFlag(DMA1_FLAG_TC1);
        
        //Collect red
        GPIO_SetBits(GPIOB,GPIO_Pin_11);
        _delay_us(DELAY_US);
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
        for(u8 i=0;i<16;i++)
        {
            if(max->red_reading[i] < (ADC_val[i] - max->off_reading[i]))
                max->red_reading[i] = ADC_val[i];
        }
        GPIO_ResetBits(GPIOB,GPIO_Pin_11);
        DMA_ClearFlag(DMA1_FLAG_TC1);

        //Collect Green
        GPIO_SetBits(GPIOB,GPIO_Pin_12);
        _delay_us(DELAY_US);
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
        for(u8 i=0;i<16;i++)
        {
            if(max->green_reading[i] < (ADC_val[i] - max->off_reading[i]))
                max->green_reading[i] = ADC_val[i];
        }
        GPIO_ResetBits(GPIOB,GPIO_Pin_12);
        DMA_ClearFlag(DMA1_FLAG_TC1);
        
        //Collect blue
        GPIO_SetBits(GPIOB,GPIO_Pin_10);
        _delay_us(DELAY_US);
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
        for(u8 i=0;i<16;i++)
        {
            if(max->blue_reading[i] < (ADC_val[i] - max->off_reading[i]))
                max->blue_reading[i] = ADC_val[i];
        }
        GPIO_ResetBits(GPIOB,GPIO_Pin_10);
        DMA_ClearFlag(DMA1_FLAG_TC1);
    }
    else{
        for(u8 i = 0 ; i < 16 ; i++){
            tempReading.off_reading[i] = 0;
            tempReading.red_reading[i] = 0;
            tempReading.green_reading[i] = 0;
            tempReading.blue_reading[i] = 0;
            tempReading.h[i] = 0;
            tempReading.s[i] = 0;
            tempReading.v[i] = 0;
        }
        
        _delay_us(DELAY_US);
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
        for(u8 i=0;i<16;i++)tempReading.off_reading[i] = (s32)ADC_val[i];
        DMA_ClearFlag(DMA1_FLAG_TC1);
        
        //Collect red
        GPIO_SetBits(GPIOB,GPIO_Pin_11);
        _delay_us(DELAY_US);
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
        for(u8 i=0;i<16;i++)
        {
            if((s32)ADC_val[i] < tempReading.off_reading[i])tempReading.red_reading[i] = tempReading.off_reading[i];
            else tempReading.red_reading[i] = (((s32)ADC_val[i] - tempReading.off_reading[i])*255)/(max_1.red_reading[i]-tempReading.off_reading[i]);
        }
        GPIO_ResetBits(GPIOB,GPIO_Pin_11);
        DMA_ClearFlag(DMA1_FLAG_TC1);

        //Collect Green
        GPIO_SetBits(GPIOB,GPIO_Pin_12);
        _delay_us(DELAY_US);
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
        for(u8 i=0;i<16;i++)
        {
            if((s32)ADC_val[i]< tempReading.off_reading[i])tempReading.green_reading[i] = tempReading.off_reading[i];
            else tempReading.green_reading[i] = (((s32)ADC_val[i] - tempReading.off_reading[i])*255)/(max_1.green_reading[i]-tempReading.off_reading[i]);
        }
        GPIO_ResetBits(GPIOB,GPIO_Pin_12);
        DMA_ClearFlag(DMA1_FLAG_TC1);
        
        //Collect blue
        GPIO_SetBits(GPIOB,GPIO_Pin_10);
        _delay_us(DELAY_US);
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
        for(u8 i=0;i<16;i++)
        {
            if((s32)ADC_val[i] < tempReading.off_reading[i])tempReading.blue_reading[i] = tempReading.off_reading[i];
            else tempReading.blue_reading[i] = (((s32)ADC_val[i] - tempReading.off_reading[i])*255)/(max_1.blue_reading[i]-tempReading.off_reading[i]);
        }
        GPIO_ResetBits(GPIOB,GPIO_Pin_10);
        DMA_ClearFlag(DMA1_FLAG_TC1);
            
        rgb_hsv_converter(&tempReading);
        
        //copy values of temp reading to the corresponding array
        for(u8 i = 0 ; i < 16 ;i++){
            calibratedHueAverage[calibrationStage - 1] += tempReading.h[i];
            calibratedSaturationAverage[calibrationStage - 1] += tempReading.s[i];
            calibratedValueAverage[calibrationStage - 1] += tempReading.v[i];
        }
        
        calibratedHueAverage[calibrationStage - 1] /= 16;
        calibratedSaturationAverage[calibrationStage - 1] /= 16;
        calibratedValueAverage[calibrationStage - 1] /= 16;

    }
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
    GPIO_SetBits(GPIOB,GPIO_Pin_11); //Red
    _delay_us(DELAY_US);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
    for(u8 i=0;i<16;i++)now.red_reading[i] = ADC_val[i];
    GPIO_ResetBits(GPIOB,GPIO_Pin_11);
    DMA_ClearFlag(DMA1_FLAG_TC1);

    //Collect Green
    GPIO_SetBits(GPIOB,GPIO_Pin_12); //Green
    _delay_us(DELAY_US);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
    for(u8 i=0;i<16;i++)now.green_reading[i] = ADC_val[i];
    GPIO_ResetBits(GPIOB,GPIO_Pin_12);
    DMA_ClearFlag(DMA1_FLAG_TC1);
    
    //Collect Blue
    GPIO_SetBits(GPIOB,GPIO_Pin_10); //Blue
    _delay_us(DELAY_US);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
    for(u8 i=0;i<16;i++)now.blue_reading[i] = ADC_val[i];
    GPIO_ResetBits(GPIOB,GPIO_Pin_10);
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


void analysisData(){
    hueAverage = 0;
    s32 minDiff = 10000;
    s32 diff;
	u8 counter = 0;
   
    for(u8 i = 0 ; i < 16 ; i++){
        if(now.s[i] >= (calibratedSaturationAverage[currentZone] - (u8)SATOFFSET)){
            hueAverage += now.h[i];
            counter++;
        }   
    } 
    if(counter != 0)
        hueAverage /= counter;
    for(u8 i = 0; i < NUMOFAREAS ; i++){
        diff = Abs(calibratedHueAverage[i] - hueAverage);
        if(diff < minDiff){
            currentZone = i;
            minDiff = diff;
        }
        if(!first_scan)
            first_scan = true;
    }
    for(u8 i = 0; i < 16 ;i++){
        if(now.s[i] >= (calibratedSaturationAverage[currentZone] - (u8)SATOFFSET))
            sat[i] = 0;
        else 
            sat[i] = 1;
    }
}

void sendData(){
	CAN_MESSAGE msg;
	msg.id = 0x0C5;
	msg.length = 8;
	for(int i=0;i<8;i++)msg.data[i] = sat[i];
	
	CAN_MESSAGE msg1;
	msg1.id = 0x0C6;
	msg1.length = 8;
	for(int i=0;i<8;i++)msg1.data[i] = sat[i+8];
    
    CAN_MESSAGE msg2;
    msg2.id = 0x0C7;
    msg2.length = 1;
    msg2.data[0] = currentZone;
    can_tx_enqueue(msg);
    can_tx_enqueue(msg1);
    can_tx_enqueue(msg2);
}


void printInformation(){
    if (get_full_ticks() % 200 == 0){
        printf("Sensor:%d\n",0);
        printf("N : %d \tR: %d\tG: %d\tB: %d\r\n",max_1.off_reading[0],max_1.red_reading[0], max_1.green_reading[0], max_1.blue_reading[0]);
        printf("N : %d \tR: %d\tG: %d\tB: %d\r\n",now.off_reading[0], now.red_reading[0], now.green_reading[0], now.blue_reading[0]);
        for(u8 i = 0 ; i < 16 ; i++){
            printf("%d:%d\n",i,now.s[i]);
        }
        printf("[hsv] H(360): %d\tS(100): %d\tV(100): %d\r\n",now.h[0],now.s[0],now.v[0]);
        printf("\n");
        printf("hueavg: %d",hueAverage);
//        for(u8 i = 0; i < 5 ; i++)printf("hue%d:%d\n",i,calibratedHueAverage[i]);
//        printf("\n");
//        for(u8 i = 0; i < 5 ; i++)printf("sat%d:%d\n",i,calibratedSaturationAverage[i]);
//        printf("\n");
//        for(u8 i = 0; i < 5 ; i++)printf("value%d:%d\n",i,calibratedValueAverage[i]);
        printf("\n");
        printf("currentzone:%d",currentZone);
    }
}

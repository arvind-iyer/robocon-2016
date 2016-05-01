#include "lightSensor.h"

volatile extern u16 ADC_val[16];
Reading now;
u8 sat[16] = {0};

//First index is areas
//Second index is mid/background
//Third index is rgb
u16 reading_in_area[REGIONS][2][3];

u16 compensated_region_color[REGIONS][3];

void init_all_zero(){
	for(u8 i = 0 ; i < 16 ; i++){
		now.off_reading[i] = 0;
		now.red_reading[i] = 0;
		now.green_reading[i] = 0;
		now.blue_reading[i] = 0;
	}
	
	for (u8 i=0;i<REGIONS;i++){
		for (u8 k=0;k<3;k++){
			compensated_region_color[i][k] = 0;
			for (u8 j=0;j<2;j++){
				reading_in_area[i][j][k] = 0;
			}
		}
	}
}

Reading this_readings[SAMPELS_TIMES];
void sensor_init(u8 cali_stage){
	
	for (u8 i=0;i<40;i++){
		
		//Collect off reading
		_delay_us(DELAY_US);
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
		for(u8 k=0;k<16;k++){
			this_readings[i].off_reading[k] = ADC_val[k];
		}
		DMA_ClearFlag(DMA1_FLAG_TC1);
		
		//Collect red reading
		GPIO_SetBits(GPIOB,GPIO_Pin_11);
		_delay_us(DELAY_US);
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
		for(u8 k=0;k<16;k++){
			this_readings[i].red_reading[k] = ADC_val[k] - this_readings[i].off_reading[k];
		}
		GPIO_ResetBits(GPIOB,GPIO_Pin_11);
		DMA_ClearFlag(DMA1_FLAG_TC1);

		//Collect green reading
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
		_delay_us(DELAY_US);
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
		for(u8 k=0;k<16;k++){
			this_readings[i].green_reading[k] = ADC_val[k] - this_readings[i].off_reading[k];
		}
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		DMA_ClearFlag(DMA1_FLAG_TC1);
		
		//Collect blue reading
		GPIO_SetBits(GPIOB,GPIO_Pin_10);
		_delay_us(DELAY_US);
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
		for(u8 k=0;k<16;k++){
			this_readings[i].blue_reading[k] = ADC_val[k] - this_readings[i].off_reading[k];
		}
		GPIO_ResetBits(GPIOB,GPIO_Pin_10);
		DMA_ClearFlag(DMA1_FLAG_TC1);
	}
	
	//Process the data into background and line reading
	
	//Do red
	s32 sum_of_red_bg = 0;
	s32 sum_of_red_mid = 0;
	for (u8 i=0;i<SAMPELS_TIMES;i++){
		for (u8 k=0;k<16;k++){
			if (k==8) continue;
			sum_of_red_bg += this_readings[i].red_reading[k];
		}
		sum_of_red_mid += (this_readings[i].red_reading[7] + this_readings[i].red_reading[8]*3 + this_readings[i].red_reading[9]);
	}
	
	//Do blue
	s32 sum_of_blue_bg = 0;
	s32 sum_of_blue_mid = 0;
	for (u8 i=0;i<SAMPELS_TIMES;i++){
		for (u8 k=0;k<16;k++){
			if (k==8) continue;
			sum_of_blue_bg += this_readings[i].blue_reading[k];
		}
		sum_of_blue_mid += (this_readings[i].blue_reading[7] + this_readings[i].blue_reading[8]*3 + this_readings[i].blue_reading[9]);
	}
	
	//Do green
	s32 sum_of_green_bg = 0;
	s32 sum_of_green_mid = 0;
	for (u8 i=0;i<SAMPELS_TIMES;i++){
		for (u8 k=0;k<16;k++){
			if (k==8) continue;
			sum_of_green_bg += this_readings[i].green_reading[k];
		}
		sum_of_green_mid += (this_readings[i].green_reading[7] + this_readings[i].green_reading[8]*3 + this_readings[i].green_reading[9]);
	}
	
	reading_in_area[cali_stage][0][0] = sum_of_red_mid / SAMPELS_TIMES / 5;
	reading_in_area[cali_stage][0][1] = sum_of_blue_mid / SAMPELS_TIMES / 5;
	reading_in_area[cali_stage][0][2] = sum_of_green_mid / SAMPELS_TIMES / 5;
	
	reading_in_area[cali_stage][1][0] = sum_of_red_bg / SAMPELS_TIMES / 15;
	reading_in_area[cali_stage][1][1] = sum_of_blue_bg / SAMPELS_TIMES / 15;
	reading_in_area[cali_stage][1][2] = sum_of_green_bg / SAMPELS_TIMES / 15;
	
	for (u8 i=0;i<3;i++){
		compensated_region_color[cali_stage][i] = (reading_in_area[cali_stage][0][i] + reading_in_area[cali_stage][1][i])/2;
	}
	
	_delay_ms(300);
}

void dataCollect(){
	//Collect off reading
	_delay_us(DELAY_US);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
	for(u8 i=0;i<16;i++){
		now.off_reading[i] = ADC_val[i];
	}
	DMA_ClearFlag(DMA1_FLAG_TC1);

	//Collect Red
	GPIO_SetBits(GPIOB,GPIO_Pin_11); //Red
	_delay_us(DELAY_US);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
	for(u8 i=0;i<16;i++){
		now.red_reading[i] = ADC_val[i] - now.off_reading[i];
	}
	GPIO_ResetBits(GPIOB,GPIO_Pin_11);
	DMA_ClearFlag(DMA1_FLAG_TC1);

	//Collect Green
	GPIO_SetBits(GPIOB,GPIO_Pin_12); //Green
	_delay_us(DELAY_US);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
	for(u8 i=0;i<16;i++){
		now.green_reading[i] = ADC_val[i] - now.off_reading[i];
	}
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	DMA_ClearFlag(DMA1_FLAG_TC1);

	//Collect Blue
	GPIO_SetBits(GPIOB,GPIO_Pin_10); //Blue
	_delay_us(DELAY_US);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
	for(u8 i=0;i<16;i++){
		now.blue_reading[i] = ADC_val[i] - now.off_reading[i];
	}
	GPIO_ResetBits(GPIOB,GPIO_Pin_10);
	DMA_ClearFlag(DMA1_FLAG_TC1);
}

void sendData(){
	
	s32 average[3] = {0}; //rgb
	
	for (u8 i=0;i<16;i++){
		average[0] += now.red_reading[i];
		average[1] += now.green_reading[i];
		average[2] += now.red_reading[i];
	}
	
	for (u8 i=0;i<3;i++){
		average[i] /= 16;
	}
	
	s32 max_diff = 0;
	u8 current_region = 0;
	for (u8 i=0;i<REGIONS;i++){
		s32 curr_diff = 0;
		for (u8 k=0;k<3;k++){
			curr_diff += abs(compensated_region_color[i][k] - average[k]);
		}
		if (curr_diff > max_diff){
			max_diff = curr_diff;
			current_region = i;
		}
	}
	
	for(u8 i = 0; i < 16; i++){
		if((now.red_reading[i] + now.blue_reading[i] + now.green_reading[i]) >
				(compensated_region_color[current_region][0]
					+ compensated_region_color[current_region][1]
					+ compensated_region_color[current_region][2])){
			sat[i] = 1;
		}else{
			sat[i] = 0;
		}
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
    
//	CAN_MESSAGE msg2;
//	msg2.id = 0x0C7;
//	msg2.length = 3;
//	msg2.data[1] = hueAverage;
//	msg2.data[2] = border;
//	if(hueAverage >= 210 && hueAverage <= 230){
//		msg2.data[0] = 1;
//	}else{
//		msg2.data[0] = 0;
//	}
//	can_tx_enqueue(msg2);
}

s32 last_ticks = 0;

void printInformation(){
	u8 index = 8;
	if (get_full_ticks() - last_ticks > 200){
		last_ticks = get_full_ticks();
		printf("RGM :%d %d %d", reading_in_area[0][0][0], reading_in_area[0][0][1], reading_in_area[0][0][2]);
		printf("RGB :%d %d %d", reading_in_area[0][1][0], reading_in_area[0][1][1], reading_in_area[0][1][2]);
		printf("CRG: %d %d %d", compensated_region_color[0][0], compensated_region_color[0][1], compensated_region_color[0][2]);
		printf("N: %d\tR: %d\tG: %d\tB: %d\r\n",now.off_reading[index], now.red_reading[index], now.green_reading[index], now.blue_reading[index]);
		printf("\n");
	}
}

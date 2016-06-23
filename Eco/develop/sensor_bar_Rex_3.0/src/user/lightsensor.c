#include "lightSensor.h"

volatile extern u16 ADC_val[16];
Reading now;
Reading full_white_reading;
u8 sat[16] = {0};

//First index is areas
//Second index is mid/background
//Third index is rgb
s16 reading_in_area[REGIONS][2][3];

s16 compensated_region_color[REGIONS][3];
s16 region_color_average[REGIONS][3];

s16 sensor_max[16] = {0};

void init_all_zero(){
	for(u8 i = 0 ; i < 16 ; i++){
		now.off_reading[i] = 0;
		now.red_reading[i] = 0;
		now.green_reading[i] = 0;
		now.blue_reading[i] = 0;
		
		full_white_reading.off_reading[i] = 0;
		full_white_reading.red_reading[i] = 0;
		full_white_reading.green_reading[i] = 0;
		full_white_reading.blue_reading[i] = 0;
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

void sensor_cali_get_reading(Reading *reading){
	//Collect off reading
	_delay_us(DELAY_US);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
	for(u8 k=0;k<16;k++){
		reading->off_reading[k] = ADC_val[k];
	}
	DMA_ClearFlag(DMA1_FLAG_TC1);

	//Collect red reading
	GPIO_SetBits(GPIOB,GPIO_Pin_11);
	_delay_us(DELAY_US);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
	for(u8 k=0;k<16;k++){
		reading->red_reading[k] = ADC_val[k] - reading->off_reading[k];
	}
	GPIO_ResetBits(GPIOB,GPIO_Pin_11);
	DMA_ClearFlag(DMA1_FLAG_TC1);

	//Collect green reading
	GPIO_SetBits(GPIOB,GPIO_Pin_12);
	_delay_us(DELAY_US);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
	for(u8 k=0;k<16;k++){
		reading->green_reading[k] = ADC_val[k] - reading->off_reading[k];
	}
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	DMA_ClearFlag(DMA1_FLAG_TC1);

	//Collect blue reading
	GPIO_SetBits(GPIOB,GPIO_Pin_10);
	_delay_us(DELAY_US);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
	for(u8 k=0;k<16;k++){
		reading->blue_reading[k] = ADC_val[k] - reading->off_reading[k];
	}
	GPIO_ResetBits(GPIOB,GPIO_Pin_10);
	DMA_ClearFlag(DMA1_FLAG_TC1);
}

void sensor_max_cali(){
	Reading this_readings[SAMPELS_TIMES];
	
	for (u8 i=0;i<SAMPELS_TIMES;i++){
		sensor_cali_get_reading(&this_readings[i]);
		for (u8 k=0; k<16; k++){
			if (this_readings[i].red_reading[k] > full_white_reading.red_reading[k]){
				full_white_reading.red_reading[k] = this_readings[i].red_reading[k];
			}
			
			if (this_readings[i].blue_reading[k] > full_white_reading.blue_reading[k]){
				full_white_reading.blue_reading[k] = this_readings[i].blue_reading[k];
			}
			
			if (this_readings[i].green_reading[k] > full_white_reading.green_reading[k]){
				full_white_reading.green_reading[k] = this_readings[i].green_reading[k];
			}
		}
	}
}

s16 get_norm_rgb(Reading *reading, u8 i, u8 rgb){
	switch (rgb){
		case 0:
			return (reading->red_reading[i])*255 / full_white_reading.red_reading[i];
		case 1:
			return (reading->green_reading[i])*255 / full_white_reading.green_reading[i];
		case 2:
			return (reading->blue_reading[i])*255 / full_white_reading.blue_reading[i];
		default:
			return 0;
	}
}

void sensor_cali(u8 cali_stage){
	
	Reading this_readings[SAMPELS_TIMES];
	
	for (u8 i=0;i<SAMPELS_TIMES;i++){
		sensor_cali_get_reading(&this_readings[i]);
	}
	
	//Process the data into background and line reading
	
	s32 sum_of_all[3] = {0};
	
	s32 sum_of_bg[3] = {0};
	s32 sum_of_mid[3] = {0};
	
	for (u8 color=0; color<3; color++){
		for (u8 i=0;i<SAMPELS_TIMES;i++){
			for (u8 k=0;k<16;k++){
				sum_of_all[color] += get_norm_rgb(&this_readings[i], k, color);
				if (k==7 || k==8) continue;
				sum_of_bg[color] += get_norm_rgb(&this_readings[i], k, color);
			}
			sum_of_mid[color] += (get_norm_rgb(&this_readings[i], 6, color) + get_norm_rgb(&this_readings[i], 7, color)*3 
												+ get_norm_rgb(&this_readings[i], 8, color)*3 + get_norm_rgb(&this_readings[i], 9, color));
		}
	}
	
	for (u8 i=0;i<3;i++){
		reading_in_area[cali_stage][0][i] = sum_of_mid[i] / SAMPELS_TIMES / 8;
		reading_in_area[cali_stage][1][i] = sum_of_bg[i] / SAMPELS_TIMES / 14;
	}
	
	for (u8 i=0;i<3;i++){
		compensated_region_color[cali_stage][i] = reading_in_area[cali_stage][1][i] + (reading_in_area[cali_stage][0][i] - reading_in_area[cali_stage][1][i]);
		region_color_average[cali_stage][i] = reading_in_area[cali_stage][1][i];
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
	u16 sensor_diff[16] = {0};
	
	u32 min_diff = (u32)-1;
	u8 current_region = 0;
	for (u8 i=0;i<REGIONS;i++){
		u32 curr_diff = 0;
		for (u8 k=0;k<16;k++){
			for (u8 color=0; color<3; color++){
				sensor_diff[k] += abs(region_color_average[i][color] - get_norm_rgb(&now, k, color));
			}
		}
		u16 first_max_diff = sensor_diff[0];
		u16 second_max_diff = 0;
		for (u8 k=1;k<16;k++){
			if (sensor_diff[k] > first_max_diff){
				second_max_diff = first_max_diff;
				first_max_diff = sensor_diff[k];
			}else if(sensor_diff[k] > second_max_diff){
				second_max_diff = sensor_diff[k];
			}
		}
		
		for (u8 k=0;k<16;k++){
			if (sensor_diff[k] != first_max_diff && sensor_diff[k] != second_max_diff){
				curr_diff += sensor_diff[k];
			}
		}
		
		if (curr_diff < min_diff){
			min_diff = curr_diff;
			current_region = i;
		}
	}
	
	for(u8 i = 0; i < 16; i++){
		if(((get_norm_rgb(&now, i, 0)) + (get_norm_rgb(&now, i, 1)) + (get_norm_rgb(&now, i, 2))) >
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
    
	CAN_MESSAGE msg2;
	msg2.id = 0x0C7;
	msg2.length = 1;
	msg2.data[0] = current_region / CALI_PRE_COLOR;
	can_tx_enqueue(msg2);
}

s32 last_ticks = 0;

void printInformation(){
	u8 index = 8;
	if (get_full_ticks() - last_ticks > 200 && !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)){
		last_ticks = get_full_ticks();
		printf("N: %d\tR: %d\tG: %d\tB: %d\r\n",now.off_reading[index], now.red_reading[index], now.green_reading[index], now.blue_reading[index]);
		printf("Stage: %d\n", cali_stage);
		for (u8 i=0;i<5;i++){
			printf("RGM :%d %d %d\n", reading_in_area[i][0][0], reading_in_area[i][0][1], reading_in_area[i][0][2]);
			printf("RGB :%d %d %d\n", reading_in_area[i][1][0], reading_in_area[i][1][1], reading_in_area[i][1][2]);
			printf("CRG: %d %d %d\n", compensated_region_color[i][0], compensated_region_color[i][1], compensated_region_color[i][2]);
			printf("RAG: %d %d %d\n", region_color_average[i][0], region_color_average[i][1], region_color_average[i][2]);
			printf("\n");
		}
	}
}

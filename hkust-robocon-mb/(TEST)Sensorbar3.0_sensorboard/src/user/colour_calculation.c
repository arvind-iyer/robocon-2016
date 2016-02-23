#include "colour_calculation.h"

u8 turn = 0;
u8 data_collect_flag;
u8 normalized_flag = 0;

extern u8 LED_STATE;
extern u8 buttonA, buttonB;

struct Reading temp_reading[max_turn];
struct Reading rgb_cap;
struct Reading calculated;
struct Reading hsv;
struct Reading output; 
struct Reading colour_list[list_num];

struct Reading* temp_reading_ptr = temp_reading;
struct Reading* rgb_cap_ptr = &rgb_cap;
struct Reading* calculated_ptr = &calculated;
struct Reading* hsv_ptr = &hsv;
struct Reading* output_ptr = &output;
struct Reading* colour_list_ptr = colour_list;

/**
	* @brief This function is used for sampling the ADC_val for nth times and take the mean of the readings, then convert the readings to hsv colour space
**/
void data_collect()
{
	if(turn==0)
	{
		for(u8 i=0;i<max_turn;i++)
			reading_init(temp_reading_ptr+i);
		reading_init(calculated_ptr);
	}
	
	set_temp_readings(temp_reading_ptr, turn, LED_STATE);
	if(LED_STATE == BLUE)
		turn++;
	
	if(turn==(max_turn - 1))
	{
		avg_temp_readings(temp_reading_ptr, calculated_ptr, max_turn);
		reading_ratio(calculated_ptr);
		
		if(normalized_flag)
		{
			rgb_normalization(calculated_ptr);
			rgb_to_hsv_degree(calculated_ptr);
		}
		
		copy_readings(hsv_ptr, output_ptr,1);
		
		turn = 0;
		data_collect_flag = 1;
	}
}
/**
	* @breif initializing the colour_list, which stores the hsv reading of different colour
**/
void colour_init()
{
	u8 finish = 0;
	
	LED_Control(1,1,1);
	while(get_ticks()%100!=0);
	LED_Control(1,1,1);
	while(get_ticks()%100!=0);
	LED_Control(1,1,1);
	while(get_ticks()%100!=0);
	LED_Control(1,1,1);
	
	set_colour(OFF);
	while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15));
	while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15));
	rgb_init();
	IndicatorControl(14);
	
//	while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15));
//	while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15));
//	set_colour_list(1);
//	IndicatorControl(16);
//	
//	while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15));
//	while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15));
//	set_colour_list(2);
//	IndicatorControl(18);
//	
//	while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15));
//	while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15));
//	set_colour_list(3);
//	IndicatorControl(20);
	set_colour(OFF);
	while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15));
	while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15));
}

void rgb_init()
{
	LED_STATE = 0;
	turn = 0;
	data_collect_flag = 0;
	while(!data_collect_flag)
	{
		set_colour(LED_STATE);
		_delay_ms(sample_time);
		ADC_Cmd(ADC1, ENABLE);
		
		while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
		data_collect();
		
		LED_STATE++;
		if(LED_STATE>2)
			LED_STATE = 0;
		DMA_ClearFlag(DMA1_FLAG_TC1);
	}
	
	copy_readings(calculated_ptr, rgb_cap_ptr, 1);
	
//	for(u8 i=0;i<16;i++)
//	{
//		rgb_cap_ptr->red_reading[i] += 3;
//		rgb_cap_ptr->green_reading[i] += 3;
//		rgb_cap_ptr->blue_reading[i] += 3;
//	}
	normalized_flag = 1;
}

//-------------------------------------------------------------Struct Manipulation--------------------------------------------------------------------------

void set_colour_list(u8 index)
{
	LED_STATE = 0;
	turn = 0;
	data_collect_flag = 0;
	while(!data_collect_flag)
	{
		set_colour(LED_STATE);
		_delay_ms(delay_time);
		ADC_Cmd(ADC1, ENABLE);
		
		while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
		data_collect();
		
		LED_STATE++;
		if(LED_STATE>2)
			LED_STATE = 0;
		DMA_ClearFlag(DMA1_FLAG_TC1);
	}
//	set_red(colour_list_ptr+index, temp_reading_ptr->red_reading);
//	set_blue(colour_list_ptr+index, temp_reading_ptr->blue_reading);
//	set_green(colour_list_ptr+index, temp_reading_ptr->green_reading);
// 	write codes to set the reading
	copy_readings(colour_list_ptr+index, calculated_ptr, 1);
	set_colour(OFF);
}

void reading_ratio(struct Reading* reading)
{
	for(u8 i=0;i<16;i++)
	{
		reading->red_reading[i] = ((reading->red_reading[i]) * 255) / max_reading;
		reading->green_reading[i] = ((reading->green_reading[i]) * 255) / max_reading;
		reading->blue_reading[i] = ((reading->blue_reading[i]) * 255) / max_reading;
	}
}

void rgb_normalization(struct Reading* reading)
{
	for(u8 i=0;i<16;i++)
	{
		reading->red_reading[i] = ((reading->red_reading[i]) * 255) / (rgb_cap_ptr->red_reading[i]);
		reading->green_reading[i] = ((reading->green_reading[i]) * 255) / (rgb_cap_ptr->green_reading[i]);
		reading->blue_reading[i] = ((reading->blue_reading[i]) * 255) / (rgb_cap_ptr->blue_reading[i]);
		
		if(reading->red_reading[i]>255)
			reading->red_reading[i] = 255;
		if(reading->green_reading[i]>255)
			reading->green_reading[i] = 255;
		if(reading->blue_reading[i]>255)
			reading->blue_reading[i] = 255;
	}
	
	
}

//-------------------------------------------------------------CAN TX MESSAGE--------------------------------------------------------------------------------

/**
	* @breif sending the successive 4 sensor reading via CAN
	* @param reading: the struct that going to read
	* @param position: the position of the a group of successive sensor (0-3), PD0-3: 0, PD4-7: 1, PD8-11: 2, PD12-15: 3
**/
void read_reading(struct Reading* reading, u8 colour, u8 position)
{
	CAN_MESSAGE msg;
	msg.id = 0x0C5;
	msg.length = 8;
	
	switch(colour)
	{
		case(RED):
			switch(position)
			{
				case 0:
					for(u8 j=0;j<2;j++)
						msg.data[j] = Two_Bytes_Decomposition(reading->red_reading[10], j);
					for(u8 j=0;j<2;j++)
						msg.data[2+j] = Two_Bytes_Decomposition(reading->red_reading[11], j);
					for(u8 j=0;j<2;j++)
						msg.data[4+j] = Two_Bytes_Decomposition(reading->red_reading[12], j);
					for(u8 j=0;j<2;j++)
						msg.data[6+j] = Two_Bytes_Decomposition(reading->red_reading[13], j);
					//IndicatorControl(0);
				break;
				
				case 1:
					for(u8 j=0;j<2;j++)
						msg.data[j] = Two_Bytes_Decomposition(reading->red_reading[0], j);
					for(u8 j=0;j<2;j++)
						msg.data[2+j] = Two_Bytes_Decomposition(reading->red_reading[1], j);
					for(u8 j=0;j<2;j++)
						msg.data[4+j] = Two_Bytes_Decomposition(reading->red_reading[2], j);
					for(u8 j=0;j<2;j++)
						msg.data[6+j] = Two_Bytes_Decomposition(reading->red_reading[3], j);
					//IndicatorControl(10);
				break;
					
				case 2:
					for(u8 j=0;j<2;j++)
						msg.data[j] = Two_Bytes_Decomposition(reading->red_reading[4], j);
					for(u8 j=0;j<2;j++)
						msg.data[2+j] = Two_Bytes_Decomposition(reading->red_reading[5], j);
					for(u8 j=0;j<2;j++)
						msg.data[4+j] = Two_Bytes_Decomposition(reading->red_reading[6], j);
					for(u8 j=0;j<2;j++)
						msg.data[6+j] = Two_Bytes_Decomposition(reading->red_reading[7], j);
					//IndicatorControl(20);
				break;
					
				case 3:
					for(u8 j=0;j<2;j++)
						msg.data[j] = Two_Bytes_Decomposition(reading->red_reading[14], j);
					for(u8 j=0;j<2;j++)
						msg.data[2+j] = Two_Bytes_Decomposition(reading->red_reading[15], j);
					for(u8 j=0;j<2;j++)
						msg.data[4+j] = Two_Bytes_Decomposition(reading->red_reading[8], j);
					for(u8 j=0;j<2;j++)
						msg.data[6+j] = Two_Bytes_Decomposition(reading->red_reading[9], j);
					//IndicatorControl(30);
				break;
			}
		break;
			
		case(BLUE):
			switch(position)
			{
				case 0:
					for(u8 j=0;j<2;j++)
						msg.data[j] = Two_Bytes_Decomposition(reading->blue_reading[10], j);
					for(u8 j=0;j<2;j++)
						msg.data[2+j] = Two_Bytes_Decomposition(reading->blue_reading[11], j);
					for(u8 j=0;j<2;j++)
						msg.data[4+j] = Two_Bytes_Decomposition(reading->blue_reading[12], j);
					for(u8 j=0;j<2;j++)
						msg.data[6+j] = Two_Bytes_Decomposition(reading->blue_reading[13], j);
					//IndicatorControl(0);
				break;
				
				case 1:
					for(u8 j=0;j<2;j++)
						msg.data[j] = Two_Bytes_Decomposition(reading->blue_reading[0], j);
					for(u8 j=0;j<2;j++)
						msg.data[2+j] = Two_Bytes_Decomposition(reading->blue_reading[1], j);
					for(u8 j=0;j<2;j++)
						msg.data[4+j] = Two_Bytes_Decomposition(reading->blue_reading[2], j);
					for(u8 j=0;j<2;j++)
						msg.data[6+j] = Two_Bytes_Decomposition(reading->blue_reading[3], j);
					//IndicatorControl(10);
				break;
					
				case 2:
					for(u8 j=0;j<2;j++)
						msg.data[j] = Two_Bytes_Decomposition(reading->blue_reading[4], j);
					for(u8 j=0;j<2;j++)
						msg.data[2+j] = Two_Bytes_Decomposition(reading->blue_reading[5], j);
					for(u8 j=0;j<2;j++)
						msg.data[4+j] = Two_Bytes_Decomposition(reading->blue_reading[6], j);
					for(u8 j=0;j<2;j++)
						msg.data[6+j] = Two_Bytes_Decomposition(reading->blue_reading[7], j);
					//IndicatorControl(20);
				break;
					
				case 3:
					for(u8 j=0;j<2;j++)
						msg.data[j] = Two_Bytes_Decomposition(reading->blue_reading[14], j);
					for(u8 j=0;j<2;j++)
						msg.data[2+j] = Two_Bytes_Decomposition(reading->blue_reading[15], j);
					for(u8 j=0;j<2;j++)
						msg.data[4+j] = Two_Bytes_Decomposition(reading->blue_reading[8], j);
					for(u8 j=0;j<2;j++)
						msg.data[6+j] = Two_Bytes_Decomposition(reading->blue_reading[9], j);
					//IndicatorControl(30);
				break;
			}
		break;
			
		case(GREEN):
			switch(position)
			{
				case 0:
					for(u8 j=0;j<2;j++)
						msg.data[j] = Two_Bytes_Decomposition(reading->green_reading[10], j);
					for(u8 j=0;j<2;j++)
						msg.data[2+j] = Two_Bytes_Decomposition(reading->green_reading[11], j);
					for(u8 j=0;j<2;j++)
						msg.data[4+j] = Two_Bytes_Decomposition(reading->green_reading[12], j);
					for(u8 j=0;j<2;j++)
						msg.data[6+j] = Two_Bytes_Decomposition(reading->green_reading[13], j);
					//IndicatorControl(0);
				break;
				
				case 1:
					for(u8 j=0;j<2;j++)
						msg.data[j] = Two_Bytes_Decomposition(reading->green_reading[0], j);
					for(u8 j=0;j<2;j++)
						msg.data[2+j] = Two_Bytes_Decomposition(reading->green_reading[1], j);
					for(u8 j=0;j<2;j++)
						msg.data[4+j] = Two_Bytes_Decomposition(reading->green_reading[2], j);
					for(u8 j=0;j<2;j++)
						msg.data[6+j] = Two_Bytes_Decomposition(reading->green_reading[3], j);
					//IndicatorControl(10);
				break;
					
				case 2:
					for(u8 j=0;j<2;j++)
						msg.data[j] = Two_Bytes_Decomposition(reading->green_reading[4], j);
					for(u8 j=0;j<2;j++)
						msg.data[2+j] = Two_Bytes_Decomposition(reading->green_reading[5], j);
					for(u8 j=0;j<2;j++)
						msg.data[4+j] = Two_Bytes_Decomposition(reading->green_reading[6], j);
					for(u8 j=0;j<2;j++)
						msg.data[6+j] = Two_Bytes_Decomposition(reading->green_reading[7], j);
					//IndicatorControl(20);
				break;
					
				case 3:
					for(u8 j=0;j<2;j++)
						msg.data[j] = Two_Bytes_Decomposition(reading->green_reading[14], j);
					for(u8 j=0;j<2;j++)
						msg.data[2+j] = Two_Bytes_Decomposition(reading->green_reading[15], j);
					for(u8 j=0;j<2;j++)
						msg.data[4+j] = Two_Bytes_Decomposition(reading->green_reading[8], j);
					for(u8 j=0;j<2;j++)
						msg.data[6+j] = Two_Bytes_Decomposition(reading->green_reading[9], j);
					//IndicatorControl(30);
				break;
			}
		break;
	}
	can_tx_enqueue(msg);
}


/**
	* @breif sending one sensor reading via CAN
	* @param reading: the struct that going to read
	* @param position: the position of the sensor (0-15)
**/
void read_single_reading(struct Reading* reading, u8 pos)
{
	CAN_MESSAGE msg;
	msg.id = 0x0C5;
	msg.length = 2;
	
	switch(pos)
	{
		case(0):
			for(u8 j=0;j<2;j++)
					msg.data[j] = Two_Bytes_Decomposition(reading->red_reading[0], j);
			IndicatorControl(0*2);
		break;
		
		case(1):
			for(u8 j=0;j<2;j++)
					msg.data[j] = Two_Bytes_Decomposition(reading->red_reading[1], j);
			IndicatorControl(2);
		break;
		
		case(2):
			for(u8 j=0;j<2;j++)
					msg.data[j] = Two_Bytes_Decomposition(reading->red_reading[2], j);
			IndicatorControl(4);
		break;
		
		case(3):
			for(u8 j=0;j<2;j++)
					msg.data[j] = Two_Bytes_Decomposition(reading->red_reading[3], j);
			IndicatorControl(6);
		break;
		
		case(4):
			for(u8 j=0;j<2;j++)
					msg.data[j] = Two_Bytes_Decomposition(reading->red_reading[4], j);
			IndicatorControl(8);
		break;
		
		case(5):
			for(u8 j=0;j<2;j++)
					msg.data[j] = Two_Bytes_Decomposition(reading->red_reading[5], j);
			IndicatorControl(10);
		break;
		
		case(6):
			for(u8 j=0;j<2;j++)
					msg.data[j] = Two_Bytes_Decomposition(reading->red_reading[6], j);
			IndicatorControl(12);
		break;
		
		case(7):
			for(u8 j=0;j<2;j++)
					msg.data[j] = Two_Bytes_Decomposition(reading->red_reading[7], j);
			IndicatorControl(14);
		break;
		
		case(8):
			for(u8 j=0;j<2;j++)
					msg.data[j] = Two_Bytes_Decomposition(reading->red_reading[8], j);
			IndicatorControl(16);
		break;
		
		case(9):
			for(u8 j=0;j<2;j++)
					msg.data[j] = Two_Bytes_Decomposition(reading->red_reading[9], j);
			IndicatorControl(18);
		break;
		
		case(10):
			for(u8 j=0;j<2;j++)
					msg.data[j] = Two_Bytes_Decomposition(reading->red_reading[10], j);
			IndicatorControl(20);
		break;
		
		case(11):
			for(u8 j=0;j<2;j++)
					msg.data[j] = Two_Bytes_Decomposition(reading->red_reading[11], j);
			IndicatorControl(22);
		break;
		
		case(12):
			for(u8 j=0;j<2;j++)
					msg.data[j] = Two_Bytes_Decomposition(reading->red_reading[12], j);
			IndicatorControl(24);
		break;
		
		case(13):
			for(u8 j=0;j<2;j++)
					msg.data[j] = Two_Bytes_Decomposition(reading->red_reading[13], j);
			IndicatorControl(26);
		break;
		
		case(14):
			for(u8 j=0;j<2;j++)
					msg.data[j] = Two_Bytes_Decomposition(reading->red_reading[14], j);
			IndicatorControl(28);
		break;
		
		case(15):
			for(u8 j=0;j<2;j++)
					msg.data[j] = Two_Bytes_Decomposition(reading->red_reading[15], j);
			IndicatorControl(30);
		break;
	}
	can_tx_enqueue(msg);
}

//-------------------------------------------------------------Colour difference functions-----------------------------------------------------------------
/**
	* @breif changing the rgb readings to hsv degree (0 - 360)
	* @param reading: the struct that going to read 
**/


void rgb_to_hsv_degree(struct Reading* reading)
{
	signed int h[16];
	u16 min[16];
	u16 max[16];
	u16 del_max[16];
	//red_reading is h, green_reading is s, blue_reading is v
	for(u8 i=0;i<16;i++)
	{
//		reading->red_reading[i] = ((reading->red_reading[i] * 1000) / 255); //correct to one decimal
//		reading->blue_reading[i] = ((reading->blue_reading[i] * 1000) / 255); //correct to one decimal
//		reading->green_reading[i] = ((reading->green_reading[i] * 1000) / 255); //correct to one decimal
				
		if(reading->red_reading[i] < reading->green_reading[i])
			min[i] = reading->red_reading[i];
		else
			min[i] = reading->green_reading[i];	
		if(reading->blue_reading[i] < min[i])
			min[i] = reading->blue_reading[i];

		if(reading->red_reading[i] > reading->green_reading[i])
			max[i] = reading->red_reading[i];
		else
			max[i] = reading->green_reading[i];
		if(reading->blue_reading[i] > max[i])
			max[i] = reading->blue_reading[i];
		
		del_max[i] = max[i] - min[i];
//		
//		output_ptr->red_reading[i] = reading->red_reading[i];
//		output_ptr->blue_reading[i] = reading->blue_reading[i];
//		output_ptr->green_reading[i] = reading->green_reading[i];
//		
		hsv.blue_reading[i] = max[i];
		
		if(del_max[i] < white_tolerance)
		{
			hsv.green_reading[i] = 0;
			hsv.red_reading[i] = 0;
			IndicatorControl(100);
		}
		else
		{			
			hsv.green_reading[i] = (del_max[i]*255)/ max[i];
			
			if(max[i] == reading->red_reading[i])
			{
				h[i] = (60*(reading->green_reading[i] - reading->blue_reading[i]))/ del_max[i];
				IndicatorControl(10);
			}
			else if(max[i] == reading->green_reading[i])
			{
				h[i] = 120 + ((reading->blue_reading[i] - reading->red_reading[i]) * 60/ del_max[i]);
				IndicatorControl(14);
			}
			else
			{
				h[i] = 240 +  ((reading->red_reading[i] - reading->green_reading[i]) * 60/ del_max[i]);
				IndicatorControl(18);
			}
			
			if(h[i]<0)
				h[i] += 360;
			
			hsv.red_reading[i] = h[i];
		}	
	}
}
#include "colour_calculation.h"

extern volatile u16 ADC_val[16];
extern u8 LED_STATE;
extern u8 buttonA, buttonB;

struct Reading temp_reading;
struct Reading hsv;
struct Reading output; 

struct Colour calculated_reading;
struct Colour colour_list[10];

u8 turn = 0;
u8 data_collect_flag;

struct Reading* temp_reading_ptr = &temp_reading;
struct Reading* hsv_ptr = &hsv;
struct Reading* output_ptr = &output;
struct Colour* calculated_reading_ptr = &calculated_reading;
struct Colour* colour_list_ptr = colour_list;


/**
	* @brief This function is used for sampling the ADC_val for nth times and take the mean of the readings, then convert the readings to hsv colour space
**/
void data_collect()
{
	if(turn==0)
	{
		temp_reading_init(temp_reading_ptr);
	}
	
	inc_temp_readings(temp_reading_ptr, LED_STATE);
	
	if(turn==(max_turn - 1))
	{
		avg_temp_readings(temp_reading_ptr, max_turn);
		//minus_off_readings(temp_reading_ptr); //Overflowed!
		//scale_temp_readings(temp_reading_ptr);
		//rgb_to_xyz(temp_reading_ptr);
		//xyz_to_lab(&xyz);
		rgb_to_hsv_degree(temp_reading_ptr);
		
		set_red(calculated_reading_ptr, temp_reading_ptr->red_reading);
		set_blue(calculated_reading_ptr, temp_reading_ptr->blue_reading);
		set_green(calculated_reading_ptr, temp_reading_ptr->green_reading);
		
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
	
	while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15));
	while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15));
	set_colour_list(0);
	IndicatorControl(14);
	
	while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15));
	while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15));
	set_colour_list(1);
	IndicatorControl(16);
	
	while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15));
	while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15));
	set_colour_list(2);
	IndicatorControl(18);
	
	while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15));
	while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15));
	set_colour_list(3);
	IndicatorControl(20);
	
	while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15));
	while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15));
}

//u8 colour_compare(struct Reading* reading, u8 index)
//{
//	
//	u8 difference_0	=	colour_difference(reading, colour_list_ptr, index) * 10;
//	u8 difference_1	= colour_difference(reading, colour_list_ptr+1, index) * 10; 
//	u8 difference_2	= colour_difference(reading, colour_list_ptr+2, index) * 10;
//	u8 difference_3	=	colour_difference(reading, colour_list_ptr+3, index) * 10;

//	CAN_MESSAGE msg;
//	msg.id = 0x0C5;
//	msg.length = 4;
//	msg.data[0] = difference_0;
//	msg.data[1] = difference_1;
//	msg.data[2] = difference_2;
//	msg.data[3] = difference_3;
//	can_tx_enqueue(msg);
//	
//	IndicatorControl(24);
//	
////	if(difference_0 < 23)
////		return 0;
////	else if(difference_1 < 23)
////		return 1;
////	else if(difference_2 < 23)
////		return 2;
////	else if(difference_3 < 23)
////		return 3;
////	else
////		return 99;
//}

//----------------------------------------------------------Struct Manipulation------------------------------------------------------------------------------
void inc_temp_readings(struct Reading* reading, u8 colour)
{
	switch(colour)
	{
		case(RED):
			for(u8 i=0;i<16;i++)
				reading->red_reading[i] += ADC_val[i];
		break;
			
		case(BLUE):
			for(u8 i=0;i<16;i++)
				reading->blue_reading[i] += ADC_val[i];
		break;
		
		case(GREEN):
			for(u8 i=0;i<16;i++)
				reading->green_reading[i] += ADC_val[i];
			turn++;
		break;
	}
}

void avg_temp_readings(struct Reading* reading, u8 freq)
{
	for(u8 i=0;i<16;i++)
	{
		reading->red_reading[i] = (reading->red_reading[i] / freq);
		reading->blue_reading[i] = (reading->blue_reading[i] / freq);
		reading->green_reading[i] = (reading->green_reading[i] / freq);
	}
}

void temp_reading_init(struct Reading* reading)
{
	for(u8 i=0;i<16;i++)
	{
		reading->red_reading[i] = 0;
		reading->blue_reading[i] = 0;
		reading->green_reading[i] = 0;
	}
}

void set_colour_list(u8 index)
{
	LED_STATE = 0;
	turn = 0;
	data_collect_flag = 0;
	while(!data_collect_flag)
	{
		set_colour(LED_STATE);
		_delay_us(700);
		ADC_Cmd(ADC1, ENABLE);
		
		while((DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET));
		data_collect();
		
		LED_STATE++;
		if(LED_STATE>2)
			LED_STATE = 0;
		DMA_ClearFlag(DMA1_FLAG_TC1);
	}
	set_red(colour_list_ptr+index, temp_reading_ptr->red_reading);
	set_blue(colour_list_ptr+index, temp_reading_ptr->blue_reading);
	set_green(colour_list_ptr+index, temp_reading_ptr->green_reading);
	
	set_colour(OFF);
}


//-------------------------------------------------------------CAN TX MESSAGE-----------------------------------------------------------------------------

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
					IndicatorControl(0);
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
					IndicatorControl(10);
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
					IndicatorControl(20);
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
					IndicatorControl(30);
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
					IndicatorControl(0);
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
					IndicatorControl(10);
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
					IndicatorControl(20);
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
					IndicatorControl(30);
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
					IndicatorControl(0);
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
					IndicatorControl(10);
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
					IndicatorControl(20);
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
					IndicatorControl(30);
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

//---------------------------------------------------------------Colour difference functions-----------------------------------
//u32 colour_difference(struct Reading* reading, struct Colour* list, u8 index)
//{
//	return Sqrt((s32)(Sqr(reading->red_reading[index] - list->red_reading[index])) + Sqr(reading->green_reading[index] - list->green_reading[index]) + Sqr(reading->blue_reading[index] - list->blue_reading[index]));
//}

/**
	* @breif changing the rgb readings to hsv
	* @param reading: the struct that going to read 
**/
void rgb_to_hsv(struct Reading* reading)
{
	//red_reading is h, green_reading is s, red reading is v
	for(u8 i=0;i<16;i++)
	{
		//calibrate the reading to 0.00x
		reading->red_reading[i] = ((reading->red_reading[i] * 100000) / max_reading);
		reading->blue_reading[i] = ((reading->blue_reading[i] * 100000) / max_reading);
		reading->green_reading[i] = ((reading->green_reading[i] * 100000) / max_reading);
		
		u32 min = MIN(MIN(reading->red_reading[i], reading->green_reading[i]),MIN(reading->green_reading[i], reading->blue_reading[i]));
		u32 max = MAX(MAX(reading->red_reading[i], reading->green_reading[i]),MAX(reading->green_reading[i], reading->blue_reading[i]));
		u32 del_max = max - min;
		
		
		reading->blue_reading[i] = del_max;
		hsv.blue_reading[i] = del_max;
		
		if(del_max<1000)
		{
			hsv.red_reading[i] = 0;
			hsv.green_reading[i] = 0;
		}
		else
		{
			hsv.green_reading[i] = (del_max / max)*100000;
			
			u32 del_r = ((((max - reading->red_reading[i])/6000000) + del_max/2000000)/del_max)*100000; //0.000
			u32 del_g = (((max - reading->green_reading[i]/6000000) + del_max/2000000)/del_max)*100000;
			u32 del_b = (((max- reading->blue_reading[i]/6000000) + del_max/2000000)/del_max)*100000;

			output.red_reading[i] = del_r;
			output.blue_reading[i] = del_g;
			output.green_reading[i] = del_b;
			
			if(reading->red_reading[i] == max)
				hsv.red_reading[i] = (del_b - del_g);
			else if(reading->green_reading[i] == max)
				hsv.red_reading[i] = 33333 + (del_r - del_b);
			else if(reading->blue_reading[i] == max)
				hsv.red_reading[i] = 66667 + (del_g - del_r);
			
			if(hsv.red_reading[i]<100000)
				hsv.red_reading[i] += 100000;
			if(hsv.red_reading[i]>100000)
				hsv.red_reading[i] -= 100000;
		}
	}
}

/**
	* @breif changing the rgb readings to hsv degree (0 - 360)
	* @param reading: the struct that going to read 
**/
void rgb_to_hsv_degree(struct Reading* reading)
{
	//red_reading is h, green_reading is s, red reading is v
	for(u8 i=0;i<16;i++)
	{
		reading->red_reading[i] = ((reading->red_reading[i] * 100000) / max_reading);
		reading->blue_reading[i] = ((reading->blue_reading[i] * 100000) / max_reading);
		reading->green_reading[i] = ((reading->green_reading[i] * 100000) / max_reading);
		
		u32 min = MIN(MIN(reading->red_reading[i], reading->green_reading[i]),MIN(reading->green_reading[i], reading->blue_reading[i]));
		u32 max = MAX(MAX(reading->red_reading[i], reading->green_reading[i]),MAX(reading->green_reading[i], reading->blue_reading[i]));
		u32 del_max = max - min;
		
		hsv.blue_reading[i] = del_max;
		
		if(del_max == 0)
		{
			hsv.green_reading[i] = 0;
			hsv.red_reading[i] = 0;
		}
			
		hsv.green_reading[i] = (del_max*100) / max;
		
		if(max == reading->red_reading[i])
			hsv.red_reading[i] = 60 * ((reading->green_reading[i] - reading->green_reading[i]) % 6);
		if(max == reading->green_reading[i])
			hsv.red_reading[i] = 60 * ((reading->blue_reading[i] - reading->red_reading[i]) / del_max) + 2;
		if(max == reading->blue_reading[i])
			hsv.red_reading[i] = 60 * ((reading->red_reading[i] - reading->green_reading[i]) / del_max) + 4;
	}
}
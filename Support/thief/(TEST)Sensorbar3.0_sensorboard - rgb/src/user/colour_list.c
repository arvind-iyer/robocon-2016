#include "colour_list.h"

extern volatile u16 ADC_val[16];

void set_temp_readings(struct Reading* reading, u8 turn, u8 colour)
{
	switch(colour)
	{
		case(RED):
			for(u8 j=0;j<16;j++)
				(reading+turn)->red_reading[j] = ADC_val[j];
		break;
		
		case(GREEN):
			for(u8 j=0;j<16;j++)
				(reading+turn)->green_reading[j] = ADC_val[j];
		break;
		
		case(BLUE):
			for(u8 j=0;j<16;j++)
				(reading+turn)->blue_reading[j] = ADC_val[j];
		break;
	}
}

void copy_readings(struct Reading* reading, struct Reading* target, u8 index)
{
	for(u8 i=0;i<index;i++)
	{
		for(u8 j=0;j<16;j++)
		{
			(target+i)->red_reading[j] = (reading+i)->red_reading[j];
			(target+i)->green_reading[j] = (reading+i)->green_reading[j];
			(target+i)->blue_reading[j] = (reading+i)->blue_reading[j];
		}
	}
}

void avg_temp_readings(struct Reading* reading, struct Reading* target, u8 max_turn)
{
	u32 temp_r[16]	= {0};
	u32 temp_g[16]	=	{0};
	u32 temp_b[16]	=	{0};
	
	for(u8 i=0;i<max_turn;i++)
	{
		for(u8 j=0;j<16;j++)
		{
			temp_r[j] += (reading+i)->red_reading[j];
			temp_g[j] += (reading+i)->green_reading[j];
			temp_b[j] += (reading+i)->blue_reading[j];
		}
	}	
	
	for(u8 j=0;j<16;j++)
	{
		target->red_reading[j] = (u16)(temp_r[j]/max_turn);
		target->green_reading[j] = (u16)(temp_g[j]/max_turn);
		target->blue_reading[j] = (u16)(temp_b[j]/max_turn);
	}
}

void reading_init(struct Reading* reading)
{
	for(u8 i=0;i<16;i++)
	{
		reading->red_reading[i] = 0;
		reading->blue_reading[i] = 0;
		reading->green_reading[i] = 0;
	}
}
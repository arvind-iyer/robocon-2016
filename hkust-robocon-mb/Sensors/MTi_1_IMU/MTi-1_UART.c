#include "MTi-1_UART.h"

float MTi_ang[3] = {0, 0, 0};
float MTi_acc[3] = {0, 0, 0};
u16 max_rx = 0;

u16 header_count = 0;	//count the header
u16 rx_count 		= 0;	//incremented by one when RX_interrupt occoured 

u8 MID = 0;
u8 d_length = 0;

//debug
u8 MTi_1_tx_data[254] = {0};
u8 *tx_ptr = MTi_1_tx_data;
u8 cs = 0;

void MTi_1_UART_init(void)
{
	uart_init(COM2, MTi_1_default_BR);
	uart_interrupt_init(COM2, MTi_1_UART_Rx);
	RCC_AHB1PeriphClockCmd(MTi_1_UART_GPIO_RCC, ENABLE);
	
	GPIO_InitTypeDef GPIOInitStructure;
	GPIOInitStructure.GPIO_Pin = MTi_1_UART_RTS_Pin;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(MTi_1_UART_GPIO, &GPIOInitStructure);
}

/**
**	@descript see MT Datasheet for details
**	@para 		*data: the data going the send, MID: the message header, data_length: no of bytes of the data
**	
**/
void send_MTi_1_UART_msg(u8 *data, u8 MID, u16 data_length)
{
	while(GPIO_ReadInputDataBit(MTi_1_UART_GPIO, MTi_1_UART_RTS_Pin) != Bit_RESET);
		
	u8 temp[data_length + 5];
	u8 checksum = 0;
	
	temp[0] = MTi_1_Preamble;
	temp[1] = MTi_1_MasterDevice;
	temp[2] = MID;
	temp[3] = data_length;
	
	for(u8 i=4; i<4+data_length; i++)
	{
		temp[i] = *(data + i - 4);
	}
	
	for(u8 i=1; i<data_length + 4; i++)
		checksum += temp[i];
	
	checksum = 0xFF - checksum + 0x01;
	cs = checksum;
	temp[data_length + 4] = checksum;
	
	for(u8 k=0; k<(data_length + 5); k++)
	{
		uart_tx_byte(COM2, temp[k]);
		MTi_1_tx_data[k] = temp[k];
	}
}

float get_MTi_ang(u8 index)
{
	return MTi_ang[index];
}

float get_MTi_acc(u8 index)
{
	return MTi_acc[index];
}

void MTi_1_UART_Rx(u8 data)
{
	static u32 raw_buffer 	= 0;	//buffer for receving data	
	static u16 data_length 	= 5;	
	static u8  ANG_COUNT 		= 0;	//counter for receiving Pdata
	static u8  ACC_COUNT 		= 0;
	
	rx_count++;
	
	if(data == MTi_1_Preamble && rx_count == 1)
		header_count++;
	else if(data == MTi_1_MasterDevice && rx_count == 2)
		header_count++;
	else if(rx_count == 3 && header_count == 2)
	{
		header_count++;
		MID = data;
	}
	else if(header_count == 3 && MID == MTData2)
	{
		if(rx_count == 4)
			data_length = data;
		
		if(rx_count == 19 && data == 0x0C)
		{
			ANG_COUNT = 12;
		}
		
		if(ANG_COUNT && rx_count >= 20)
		{
			if(ANG_COUNT > 8)
			{
				raw_buffer = (raw_buffer >> 8) | data; 
				ANG_COUNT--;
				if(ANG_COUNT == 8)
				{
					MTi_ang[0] = (float)raw_buffer;
					raw_buffer = 0;
				}
			}
			else if(ANG_COUNT > 4 && ANG_COUNT <= 8)
			{
				raw_buffer = (raw_buffer >> 8) | data;
				ANG_COUNT--;
				if(ANG_COUNT == 4)
				{
					MTi_ang[1] = (float)raw_buffer;
					raw_buffer = 0;
				}
			}
			else
			{
				raw_buffer = (raw_buffer >> 8) | data;
				ANG_COUNT--;
				if(!ANG_COUNT)
				{
					ACC_COUNT = 12;
					MTi_ang[2] = (float)raw_buffer;
					raw_buffer = 0;
				}
			}
		}
		
		if(rx_count >= 35 && ACC_COUNT)
		{
			if(ACC_COUNT > 8)
			{
				raw_buffer = (raw_buffer >> 8) | data;
				ACC_COUNT--;
				if(ACC_COUNT == 8)
				{
					MTi_acc[0] = *(float*)&raw_buffer;
					raw_buffer = 0;
				}
			}
			else if(ACC_COUNT > 4 && ACC_COUNT <= 8)
			{
				raw_buffer = (raw_buffer >> 8) | data;
				ACC_COUNT--;
				if(ACC_COUNT == 4)
				{
					MTi_acc[1] = (float)raw_buffer;
					raw_buffer = 0;
				}
			}
			else
			{
				raw_buffer = (raw_buffer >> 8) | data;
				ACC_COUNT--;
				if(!ACC_COUNT)
				{
					MTi_acc[2] = (float)raw_buffer;
					raw_buffer = 0;
				}
			}
		}
	}
	else
	{
		rx_count = 0;
		header_count = 0;
	}
	if(rx_count == data_length + 4)
	{
		rx_count = 0;
		header_count = 0;
	}
	
	if(rx_count > max_rx)
		max_rx = rx_count;
}	
	



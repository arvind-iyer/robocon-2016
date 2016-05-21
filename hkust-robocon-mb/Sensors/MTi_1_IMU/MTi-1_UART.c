#include "MTi-1_UART.h"

u8 MTi_1_rx_data[254];
u8 MID = 0;
u8 MTi_1_flag;
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
	
	cs = checksum;
	
	checksum = 0xFF - checksum + 0x01;
	temp[data_length + 4] = checksum;
	
	for(u8 k=0; k<(data_length + 5); k++)
	{
		uart_tx_byte(COM2, temp[k]);
		MTi_1_tx_data[k] = temp[k];
	}
}

void MTi_1_UART_Rx(u8 data)
{
	static u16 header_count = 0;
	static u16 raw_length = 0;
	static u16 data_length = 0;
	
	if(data == MTi_1_Preamble)
		header_count++;
	else if(data == MTi_1_MasterDevice)
		header_count++;
	else if(raw_length == 2 && header_count == 2)
	{
		MTi_1_flag = MTi_Data_Not_Ready;
		header_count++;
		MID = data;
	}
	else if(raw_length == 3 && header_count == 3)
	{
		data_length = data;
		d_length = data;
		header_count = 0;
	}
	else
	{
		raw_length = 0;
	}
	
	if(data_length)
	{	
		MTi_1_rx_data[raw_length - 5] = data;
		data_length--;
		if(data_length == 0)
		{
			MTi_1_flag = MTi_Data_Ready;
			raw_length = 0;
			header_count = 0;
			return;
		}
	}
	raw_length++;
}


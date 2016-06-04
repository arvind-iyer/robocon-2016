#include "laser_sensor_cheap.h"

/**
** HKUST Robocon 2016
** $500 Laser sensor, range from 5cm to 30m
** Author: Rex Cheng
**/

static u8 laser_byte_array[2][11] = {0};
static u8 laser_byte_pointer[2] = {0};

void cheap_laser_init(){
	uart_init(CHEAP_LASER_1_UART, 9600);
	uart_interrupt(CHEAP_LASER_1_UART);
	uart_init(CHEAP_LASER_2_UART, 9600);
	uart_interrupt(CHEAP_LASER_2_UART);
	
	_delay_ms(500);
	tft_clear();
	
	//Set freq
	uart_tx_byte(COM2, 0xFA);
	uart_tx_byte(COM2, 0x04);
	uart_tx_byte(COM2, 0x0A);
	uart_tx_byte(COM2, 0x14);
	uart_tx_byte(COM2, 0xE4);
	pointer = 0;
	tft_println("S1");
	tft_update();
	
	_delay_ms(450);
	
	while (byte_array[0] != 0xFA ||
		byte_array[1] != 0x04 ||
		byte_array[2] != 0x8A ||
		byte_array[3] != 0x78);
	
	//Set range
	uart_tx_byte(COM2, 0xFA);
	uart_tx_byte(COM2, 0x04);
	uart_tx_byte(COM2, 0x09);
	uart_tx_byte(COM2, 0x1E);
	uart_tx_byte(COM2, 0xDB);
	pointer = 0;
	tft_println("S2");
	tft_update();
	
	_delay_ms(450);
	
	while (byte_array[0] != 0xFA ||
		byte_array[1] != 0x04 ||
		byte_array[2] != 0x89 ||
		byte_array[3] != 0x79) {
	}
	
	//Change time
	uart_tx_byte(COM2, 0xFA);
	uart_tx_byte(COM2, 0x04);
	uart_tx_byte(COM2, 0x05);
	uart_tx_byte(COM2, 0x01);
	uart_tx_byte(COM2, 0xFC);
	pointer = 0;
	tft_println("S3");
	tft_update();
	
	_delay_ms(450);
	
	while (byte_array[0] != 0xFA ||
		byte_array[1] != 0x04 ||
		byte_array[2] != 0x85 ||
		byte_array[3] != 0x7D);
		
	//Open laser
//	uart_tx_byte(COM2, 0x80);
//	uart_tx_byte(COM2, 0x06);
//	uart_tx_byte(COM2, 0x03);
//	uart_tx_byte(COM2, 0x77);
	
	tft_println("S4");
	tft_update();
	
	tft_clear();
	pointer = 0;
	_delay_ms(500);
}

void UART3_IRQHandler(void){
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){
		u8 rx_data = (u8)USART_ReceiveData(USART3);
		laser_byte_array[0][laser_byte_pointer[0]] = rx_data;
		if (laser_byte_pointer[0] > 10){
			laser_byte_pointer[0] = 0;
		}
	}
}

void UART4_IRQHandler(void){
	if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET){
		u8 rx_data = (u8)USART_ReceiveData(UART4);
		laser_byte_array[1][laser_byte_pointer[1]] = rx_data;
		if (laser_byte_pointer[1] > 10){
			laser_byte_pointer[1] = 0;
		}
	}
}
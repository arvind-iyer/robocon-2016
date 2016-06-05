#include "laser_sensor_cheap.h"

/**
** HKUST Robocon 2016
** $500 Laser sensor, range from 5cm to 30m
** Author: Rex Cheng
**/

static u8 laser_byte_array[3][11] = {0};
static u8 laser_byte_pointer[3] = {0};

void cheap_laser_init(){
	for (u8 i=COM3;i<=COM5;i++){
		uart_init(i, 9600);
	}
	
	uart_interrupt(CHEAP_LASER_1_UART);
	uart_interrupt(CHEAP_LASER_2_UART);
	uart_interrupt(CHEAP_LASER_3_UART);
	
	_delay_ms(500);
	
	//Set freq
	for (u8 i=COM3;i<=COM5;i++){
		uart_tx_byte(i, 0xFA);
		uart_tx_byte(i, 0x04);
		uart_tx_byte(i, 0x0A);
		uart_tx_byte(i, 0x14);
		uart_tx_byte(i, 0xE4);
		laser_byte_pointer[(u8)COM3 - 2] = 0;
	}
	
	_delay_ms(450);
	
	//Set range
	for (u8 i=COM3;i<=COM5;i++){
		uart_tx_byte(i, 0xFA);
		uart_tx_byte(i, 0x04);
		uart_tx_byte(i, 0x09);
		uart_tx_byte(i, 0x1E);
		uart_tx_byte(i, 0xDB);
		laser_byte_pointer[(u8)COM3 - 2] = 0;
	}
	
	_delay_ms(450);
	
	//Change time
	for (u8 i=COM3;i<=COM5;i++){
		uart_tx_byte(i, 0xFA);
		uart_tx_byte(i, 0x04);
		uart_tx_byte(i, 0x05);
		uart_tx_byte(i, 0x01);
		uart_tx_byte(i, 0xFC);
		laser_byte_pointer[(u8)COM3 - 2] = 0;
	}
	
	_delay_ms(450);
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

void UART5_IRQHandler(void){
	if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET){
		u8 rx_data = (u8)USART_ReceiveData(UART5);
		laser_byte_array[2][laser_byte_pointer[2]] = rx_data;
		if (laser_byte_pointer[2] > 10){
			laser_byte_pointer[2] = 0;
		}
	}
}
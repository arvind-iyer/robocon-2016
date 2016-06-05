#include "laser_sensor_cheap.h"

/**
** HKUST Robocon 2016
** $500 Laser sensor, range from 5cm to 30m
** Author: Rex Cheng
**/

u8 laser_byte_array[3][100] = {65};
u8 laser_byte_pointer[3] = {0};
u32 ticks_diff_laser[3] = {0};
u32 last_ticks_laser[3] = {0};

void cheap_laser_init(){
	for (u8 i=COM3;i<=COM5;i++){
		uart_init(i, 9600);
	}
	
	uart_interrupt(CHEAP_LASER_1_UART);
	uart_interrupt(CHEAP_LASER_2_UART);
	uart_interrupt(CHEAP_LASER_3_UART);
	
	_delay_ms(100);
	
	//Change time
	for (u8 i=COM3;i<=COM5;i++){
		uart_tx_byte(i, 0xFA);
		uart_tx_byte(i, 0x04);
		uart_tx_byte(i, 0x05);
		uart_tx_byte(i, 0x00);
		uart_tx_byte(i, 0xFD);
		laser_byte_pointer[(u8)i - 2] = 0;
	}
	
	_delay_ms(100);

	//Set freq
	for (u8 i=COM3;i<=COM5;i++){
		uart_tx_byte(i, 0xFA);
		uart_tx_byte(i, 0x04);
		uart_tx_byte(i, 0x0A);
		uart_tx_byte(i, 0x14);
		uart_tx_byte(i, 0xE4);
		laser_byte_pointer[(u8)i - 2] = 0;
	}
	
	_delay_ms(100);
	
	//Start laser
	for (u8 i=COM3;i<=COM5;i++){
		uart_tx_byte(i, 0x80);
		uart_tx_byte(i, 0x06);
		uart_tx_byte(i, 0x03);
		uart_tx_byte(i, 0x77);
	}
}
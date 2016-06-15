#include "dual_cheap_laser.h"

/**
** HKUST Robocon 2016
** $500 Laser sensor, range from 5cm to 30m
** Author: Rex Cheng
**/

static u8 laser_byte_array[2][100] = {65};
static u8 laser_byte_pointer[2] = {0};
//static u32 last_ticks_laser[2] = {0};
static u32 last_init_ticks = 0;
static u8 init_stage = 0;
static bool is_ready = false;

void dual_laser_init(){
	for (u8 i=DUAL_LASER1_UART;i<=DUAL_LASER2_UART;i++){
		uart_init((COM_TypeDef)i, 9600);
	}
	
	uart_interrupt(DUAL_LASER1_UART);
	uart_interrupt(DUAL_LASER2_UART);
	
	last_init_ticks = this_loop_ticks;
	init_stage = 0;
}

void dual_laser_init_update(){
	if (this_loop_ticks - last_init_ticks > DUAL_INIT_TICKS_DELAY){
		switch (init_stage){
			case 0:
				//Change data output time
				for (u8 i=DUAL_LASER1_UART;i<=DUAL_LASER2_UART;i++){
					uart_tx_byte((COM_TypeDef)i, 0xFA);
					uart_tx_byte((COM_TypeDef)i, 0x04);
					uart_tx_byte((COM_TypeDef)i, 0x05);
					uart_tx_byte((COM_TypeDef)i, 0x00);
					uart_tx_byte((COM_TypeDef)i, 0xFD);
					laser_byte_pointer[(u8)i - 2] = 0;
				}
				init_stage++;
				break;
				
			case 1:
				//Set freq
				for (u8 i=DUAL_LASER1_UART;i<=DUAL_LASER2_UART;i++){
					uart_tx_byte((COM_TypeDef)i, 0xFA);
					uart_tx_byte((COM_TypeDef)i, 0x04);
					uart_tx_byte((COM_TypeDef)i, 0x0A);
					uart_tx_byte((COM_TypeDef)i, 0x14);
					uart_tx_byte((COM_TypeDef)i, 0xE4);
					laser_byte_pointer[(u8)i - 2] = 0;
				}
				init_stage++;
				break;
				
			case 2:
				//Start laser
				for (u8 i=DUAL_LASER1_UART;i<=DUAL_LASER2_UART;i++){
					uart_tx_byte((COM_TypeDef)i, 0x80);
					uart_tx_byte((COM_TypeDef)i, 0x06);
					uart_tx_byte((COM_TypeDef)i, 0x03);
					uart_tx_byte((COM_TypeDef)i, 0x77);
				}
				init_stage++;
				break;
				
			case 3:
				is_ready = true;
				break;
		}
		last_init_ticks = this_loop_ticks;
	}
}

bool is_dual_laser_ready(){
	return is_ready;
}

void USART3_IRQHandler(void){
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){
		u8 rx_data = (u8)USART_ReceiveData(USART3);
		laser_byte_array[0][laser_byte_pointer[0]++] = rx_data;
		if (rx_data == 0x80){
			laser_byte_pointer[0] = 1;
			//last_ticks_laser[0] = get_full_ticks();
		}
	}
}

void UART4_IRQHandler(void){
	if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET){
		u8 rx_data = (u8)USART_ReceiveData(UART4);
		laser_byte_array[1][laser_byte_pointer[1]++] = rx_data;
		if (rx_data == 0x80){
			laser_byte_pointer[1] = 1;
			//last_ticks_laser[1] = get_full_ticks();
		}
	}
}

//ascii to decimal
inline static u8 a2d(u8 ascii){
	return ascii - 48;
}

//Find the newest data and convert it
s16 get_cheap_laser_dis(u8 id){
	if (!is_ready){
		return 0;
	}
	
	if (laser_byte_array[id][3] == 'E'){
		return -1;
	}else{
		return a2d(laser_byte_array[id][3])*100000 + a2d(laser_byte_array[id][4])*10000 + a2d(laser_byte_array[id][5])*1000
				+ a2d(laser_byte_array[id][7])*100 + a2d(laser_byte_array[id][8])*10 + a2d(laser_byte_array[id][9]);
	}
}

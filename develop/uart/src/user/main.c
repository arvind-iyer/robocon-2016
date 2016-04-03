#include "main.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

u32 this_loop_ticks = 0;
u8 init = 0;
u8 state = 0;

u8 rx_state = 0;
uint8_t rx_buffer[4] = {0,0,0,0};

void rx_reset(void) {
	rx_state = 0;
}

s32 rx_merge(void) {
	s32 val = 0;
	val |= rx_buffer[0] << 0;
	val |= rx_buffer[1] << 8;
	val |= rx_buffer[2] << 16;
	val |= rx_buffer[3] << 24;
	return val;
}

int main(void) {
	servo_init();
	led_init();
	ticks_init();
	tft_easy_init();
	can_init();
	can_rx_init();
	can_motor_init();
	pneumatic_init();
	buzzer_init();
	button_init();
	
	uart_init(COM1, 115200);
	uart_interrupt(COM1);

	tft_put_logo(85, 120);       
	CONTROL_STATE last_control_state = MANUAL_MODE;
	
	while(1){
		tft_clear();
		tft_prints(0,0,"%d",rx_merge());
		tft_update();
	}
}

void USART1_IRQHandler(void) {
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
		const uint8_t byte = USART_ReceiveData(USART1);
		
		rx_buffer[rx_state] = byte;
		rx_state = (rx_state+1)%4;
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

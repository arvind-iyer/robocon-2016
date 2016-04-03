#include "main.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

u32 this_loop_ticks = 0;
u8 init = 0;
u8 state = 0;
	
uint8_t data;

s32 rx_dword(void) {
	//do something
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
		tft_prints(0,0,"RX: %d",data);
		tft_update();
	}
}

void USART1_IRQHandler(void) {
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
		const uint8_t byte = USART_ReceiveData(USART1);
		data = byte;
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

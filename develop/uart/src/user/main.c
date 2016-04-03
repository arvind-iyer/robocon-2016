#include "main.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

u32 this_loop_ticks = 0;
u8 init = 0;
u8 state = 0;

int main(void) {
	servo_init();
	led_init();
	ticks_init();
	tft_easy_init();
	can_init();
	can_rx_init();
	can_motor_init();
	uart_init(COM1, 115200);
	pneumatic_init();
	buzzer_init();
	button_init();

	tft_put_logo(85, 120);            
	CONTROL_STATE last_control_state = MANUAL_MODE;
	
	uart_tx_byte(COM1, 65);
	
	while(1){
	}
}
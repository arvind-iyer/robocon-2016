#include "main.h"


int main(void) {
	servo_init();
	led_init();
	ticks_init();
	tft_easy_init();
	can_init();
	can_rx_init();
	can_motor_init();
	uart_init(COM1, 115200);
	manual_init();
	//can_xbc_mb_tx_enable(true);

	tft_put_logo(110, 90);
	u32 lastTicks = 0;                                                                                                                                                                                                                                                                                                                                      
	
	while(1){
		if ((lastTicks - get_full_ticks())>50){
			lastTicks = get_full_ticks();
			led_blink(LED_D1);
			button_update();
			manual_control_update();
			
			tft_clear();
			tft_append_line("%d",(xbc_get_joy(XBC_JOY_RY)+120)*1050/(BURSHLESS_MAX-BURSHLESS_MIN)+BURSHLESS_MIN);
			tft_append_line("%d", get_ticks());
			tft_update();
			
		}
	}
}
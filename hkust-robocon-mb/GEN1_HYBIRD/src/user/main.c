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
	pneumatic_init();
	buzzer_init();
	//can_xbc_mb_tx_enable(true);

	tft_put_logo(110, 90);
	u32 lastTicks = 0;              
	u8 control_state = MANUAL_CONTROL;
	
	while(1){
		if ((get_full_ticks() - lastTicks)>50){
			lastTicks = get_full_ticks();
			button_update();
			if (control_state==MANUAL_CONTROL){
			control_state = manual_control_update();
			}else{
				//AUTO
			}
		}
	}
}
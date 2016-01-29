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
	//can_xbc_mb_tx_enable(true);

	tft_put_logo(110, 90);
	u32 lastTicks = 0;              
	u8 control_state = MANUAL_CONTROL;
	
	while(1){
		if ((lastTicks - get_full_ticks())>50){
			lastTicks = get_full_ticks();
			led_blink(LED_D1);
			button_update();
			tft_clear();
			tft_append_line("%d", button_pressed(BUTTON_XBC_LB));
			tft_append_line("%d", button_pressed(BUTTON_XBC_RB));
			tft_append_line("%d", button_pressed(BUTTON_XBC_START));
			tft_append_line("%d", button_pressed(BUTTON_XBC_BACK));
			tft_update();
			if (control_state==MANUAL_CONTROL){
				control_state = manual_control_update();
			}else{
				//AUTO
			}
			motor_set_vel(MOTOR4, CLIMBING_SPEED, OPEN_LOOP);
			motor_set_vel(MOTOR5, CLIMBING_SPEED, OPEN_LOOP);
			motor_set_vel(MOTOR6, CLIMBING_SPEED, OPEN_LOOP);
		}
	}
}
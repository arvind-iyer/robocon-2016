#include "main.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

u32 last_loop_ticks = 0;
u32 last_long_loop_ticks = 0;
u32 this_loop_ticks = 0;
u32 last_short_loop_ticks = 0;

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
	encoder_init();
	gyro_init();
	button_init();
	can_xbc_mb_init();
	can_xbc_mb_tx_enable(true);

	tft_put_logo(85, 120);            
	CONTROL_STATE last_control_state = MANUAL_MODE;
	
	//servo_control(SERVO3, 1500);
	
	while(1){
		this_loop_ticks = get_full_ticks();
		
		//Dont care if same ticks
		if (this_loop_ticks == last_loop_ticks){
			continue;
		}
		
		if ((this_loop_ticks - last_short_loop_ticks)>5){
			//Update the pressed state of the buttons
			button_update();
			//Get state for manual/auto and emergency lock
			xbc_global_update();
			last_short_loop_ticks = this_loop_ticks;
		}
		
		//Deal with switching control, reseting the control
		CONTROL_STATE control_state = get_control_state();
		if (control_state != last_control_state){
			if (control_state == MANUAL_MODE){
				manual_reset();
			}else{
				auto_init();
			}
		}
		last_control_state = control_state;

		if (get_emergency_lock() == UNLOCKED){
			//Update with short interval here
			if (control_state == MANUAL_MODE){
					//manual_fast_update();
			}
		}
		
		if ((this_loop_ticks - last_long_loop_ticks)>LONG_LOOP_TICKS){
			led_blink(LED_D1);
			
			servo_control(SERVO3, 1500);
			//servo_control(SERVO3, (Abs((this_loop_ticks % 10000) - 5000) / 5 + 1000));
				
			//Update with longer update interval here
			if (get_emergency_lock() == UNLOCKED){
				led_blink(LED_D2);
				
				if (control_state == MANUAL_MODE){
					manual_interval_update();
				}else{
					if (auto_get_state() == RUNNING_MODE) {
						auto_var_update();			
						auto_motor_update();
						//auto_calibrate();
					} else if (auto_get_state() == MENU_MODE) {
						auto_menu_update();
					} else if (auto_get_state() == DATA_MODE) {
						auto_data_update();
					}
				}
				can_xbc_mb_lcd_tx();
			}
			last_long_loop_ticks = this_loop_ticks;
		}
		last_loop_ticks = this_loop_ticks;
	}
}
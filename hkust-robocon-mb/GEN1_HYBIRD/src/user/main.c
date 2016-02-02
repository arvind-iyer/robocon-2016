#include "main.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

u16 last_loop_ticks = 0;
u16 last_loop_seconds = 0;
u16 last_long_loop_ticks;
u16 last_long_loop_seconds;
u16 this_loop_ticks = 0;
u16 this_loop_seconds = 0;

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
	//can_xbc_mb_tx_enable(true);

	tft_put_logo(110, 90);            
	LOCK_STATE last_emergency_lock = UNLOCKED;
	CONTROL_STATE last_control_state = MANUAL_MODE;
	
	while(1){
		this_loop_ticks = get_ticks();
		//Dont care if same ticks
		if (this_loop_ticks == last_loop_ticks){
			continue;
		}
		last_loop_ticks = this_loop_ticks;
		
		if ((this_loop_ticks - last_loop_ticks)>5){
			//Update the pressed state of the buttons
			button_update();
			//Get state for manual/auto and emergency lock
			xbc_global_update();
		}
		
		//Deal with emergency lock, to lock or to relax
		if (get_emergency_lock() != last_emergency_lock){
			if (get_emergency_lock() == LOCKED){
				manual_emergency_stop();
			}else{
				manual_emergency_relax();
			}
		}
		last_emergency_lock = get_emergency_lock();
		
		//Deal with switching control, reseting the control
		CONTROL_STATE control_state = get_control_state();
		if (control_state != last_control_state){
			if (control_state == MANUAL_MODE){
				manual_reset();
			}else{
				manual_reset();
				//Something like auto_reset() I guess?
			}
		}
		last_control_state = control_state;
		
		if (get_emergency_lock() == UNLOCKED){
	
			if (control_state == MANUAL_MODE){
					manual_fast_update();
			}
			
			if ((this_loop_ticks - last_long_loop_ticks)>50){
				last_long_loop_ticks = get_full_ticks();
				if (control_state == MANUAL_MODE){
					manual_interval_update();
				}else{
					auto_var_update();			
					auto_motor_update();
					//auto_calibrate();
				}
			}
		}
	}
}
/*
=======

int main(void)
{
	
	auto_init();
	//auto_tar_enqueue(0, 1500, 0, 0.0, true);
	
	
	//8-figure
	auto_tar_enqueue(500, 0, 0, 0.0, false);
	auto_tar_enqueue(1000, 500, 0, -2.0, false);
	auto_tar_enqueue(500, 1000, 0, -2.0, false);
	auto_tar_enqueue(0, 500, 0, -2.0, false);
	auto_tar_enqueue(0, -500, 0, 0.0, false);
	auto_tar_enqueue(-500, -1000, 0, 2.0, false);
	auto_tar_enqueue(-1000, -500, 0, 2.0, false);
	auto_tar_enqueue(-500, 0, 0, 2.0, false); 
	auto_tar_enqueue(0, 0, 0, 0.0, true);
	
	
	//circle
	auto_tar_enqueue(1000, 1000, 0, 1.0, true);
	auto_tar_enqueue(2000, 0, 0, 1.0, true);
	auto_tar_enqueue(1000, -1000, 0, 1.0, true);
	auto_tar_enqueue(0, 0, 0, 1.0, true);
}
*/
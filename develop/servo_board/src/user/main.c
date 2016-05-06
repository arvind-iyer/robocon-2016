#include "main.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

u32 this_loop_ticks = 0;
u8 init = 0;
u8 state = 0;
u16 pwm = 2048;

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
	
	i2c_init();
	init = pca9685_init();
	
	//pca9685_reset_pwm();
	/*
	state |= pca9685_set_pwm(6, 1366);
	state |= pca9685_set_pwm(10, 2048);
	state |= pca9685_set_pwm(14, 2730);
	*/
	
	while(1){
		this_loop_ticks = get_full_ticks();
		button_update();
		
		tft_clear();
		tft_prints(0,0,"Hello!");
		tft_prints(0,1,"%d",this_loop_ticks);
		tft_prints(0,2,"%d",pwm);
		
		if(button_pressed(BUTTON_1) && (pwm <= 2730)) {
			pwm += 5;
			tft_prints(0,3,"Increasing");			
		}
		if(button_pressed(BUTTON_2) && (pwm >= 1366)) {
			pwm -= 5;
			tft_prints(0,3,"Decreasing");			
		}
		
		state = pca9685_set_pwm(4, 2730);
		if (state) {
			tft_prints(0,4,"Error");
		} else {
			tft_prints(0,4,"OK!");
		}
		tft_update();
	}
}
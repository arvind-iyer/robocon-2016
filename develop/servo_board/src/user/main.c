#include "main.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

u32 this_loop_ticks = 0;
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
	
	pca9685_init();
	state = pca9685_set_pwm(0x3E, 0x99);
	
	while(1){
		this_loop_ticks = get_full_ticks();
		
		
		tft_clear();
		tft_prints(0,0,"Hello!");
		tft_prints(0,1,"%d",this_loop_ticks);
		tft_prints(0,2,"%d",state);
		tft_update();
	}
}
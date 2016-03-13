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
	
	i2c_init();
	init = pca9685_init();
	
	while(1){
		this_loop_ticks = get_full_ticks();
		
		state = pca9685_set_pwm();
		
		tft_clear();
		tft_prints(0,0,"Hello!");
		tft_prints(0,1,"%d",this_loop_ticks);
		tft_prints(0,2,"INIT: %d",init);
		tft_prints(0,3,"SET:  %d",state);
		tft_update();
	}
}
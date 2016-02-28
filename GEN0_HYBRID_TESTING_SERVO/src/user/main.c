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
	bluetooth_init();
	can_xbc_mb_init();
	debug_init();
	can_xbc_mb_tx_enable(true);

	tft_put_logo(80, 120);            
	CONTROL_STATE last_control_state = MANUAL_MODE;

	u16 servo_val = SERVO_MED;
	while(1){
		this_loop_ticks = get_full_ticks();
		
		if ((this_loop_ticks - last_long_loop_ticks)>LONG_LOOP_TICKS){
			led_blink(LED_D1);
			last_long_loop_ticks = this_loop_ticks;
		}
		
		if (button_pressed(BUTTON_1)){
			servo_val++;
			if (servo_val>SERVO_MAX) servo_val = SERVO_MAX;
			led_blink(LED_D2);
		}else	if (button_pressed(BUTTON_2)){
			servo_val--;
			if (servo_val<SERVO_MIN) servo_val = SERVO_MIN;
			led_blink(LED_D3);
		}
		button_update();
		tft_clear();
		tft_append_line("%d", servo_val);
		tft_update();
		servo_control_all(servo_val);
		last_loop_ticks = this_loop_ticks;
	}
}
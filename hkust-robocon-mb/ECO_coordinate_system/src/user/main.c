#include "main.h"

int main(void) {
	//Init
	led_init();
	tft_easy_init();
	ticks_init();
	servo_init();
	encoder_init();
	button_init();
	track_init();
	
	//brings the blink on
	led_control(LED_D1, LED_ON);
	
	while(1){
		button_update();
		
		//track_test();
		
		if (get_full_ticks()%200 == 0){
			led_blink(LED_D1 | LED_D2);
		}

		tft_clear();
		
		tft_append_line("EL:%d ER:%d", get_count(ENCODER1), get_count(ENCODER2));
		tft_append_line("VL:%d VR:%d", get_vel(ENCODER1), get_vel(ENCODER2));
		tft_append_line("ED:%d VD:%d", get_count(ENCODER1)-get_count(ENCODER2), get_vel(ENCODER1)-get_vel(ENCODER2));
		track_update();
		
		tft_update();
	}
}
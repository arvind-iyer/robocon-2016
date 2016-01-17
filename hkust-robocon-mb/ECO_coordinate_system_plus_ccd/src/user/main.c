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
	line_sensor_init();
	
	//brings the blink on
	led_control(LED_D1, LED_ON);
	
	while(1){
		button_update();
		
		//track_test();
		
		if (get_full_ticks()%200 == 0){
			led_blink(LED_D1 | LED_D2);
		}

		u8 line_sensor_state = get_line_sensor_all();
		
		tft_clear();
		tft_append_line("EL:%d ER:%d", get_count(ENCODER1), get_count(ENCODER2));
		tft_append_line("VL:%d VR:%d", get_vel(ENCODER1), get_vel(ENCODER2));
		tft_append_line("ED:%d VD:%d", get_count(ENCODER1)-get_count(ENCODER2), get_vel(ENCODER1)-get_vel(ENCODER2));
		tft_append_line(BYTETOBINARYPATTERN" %d", BYTETOBINARY(line_sensor_state), line_sensor_state);
		//track_update();
		
		tft_update();
		
		switch(line_sensor_state){
			case track_turn_happy:
				servo_control_all(SERVO_MED);
				break;
			case track_turn_left_smooth:
				//servo_control_all(1900);
				servo_control_all(2400);
				break;
			case track_turn_left_90:
				servo_control_all(2400);
				break;
			case track_turn_right_smooth:
				//servo_control_all(1200);
				servo_control_all(700);
				break;
			case track_turn_right_90:
				servo_control_all(700);
				break;
			default:
				led_control(LED_D3, LED_ON);
				break;
		}
	}
}
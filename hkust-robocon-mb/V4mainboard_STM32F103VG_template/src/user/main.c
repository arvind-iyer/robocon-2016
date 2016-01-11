#include "main.h"

#define SERVO_MIN 500
#define SERVO_MAX 2450

uint16_t servo_val = SERVO_MIN;

int main(void) {
	
	led_init();
	
	//led_control(LED_D1, 1);
	
	tft_init(1, BLUE, WHITE, RED);
	ticks_init();
	button_init();
	servo_init();
	
	tft_clear();
	led_control(LED_D1, LED_OFF);
	
	while(1){
		button_update();

		if (get_ticks()%2000 == 0){
			tft_append_line(BYTETOBINARYPATTERN, BYTETOBINARY(get_led_state()));
			led_blink(LED_D1 | LED_D2);
			tft_update();
			while(1);
		}

		/*
		
		tft_clear();
		
		tft_prints(0, y_index++, "%d %d", button_pressed(BUTTON_1), button_pressed(BUTTON_2));
		tft_prints(0, y_index++, "%d %d %d %d %d", button_pressed(BUTTON_JS_UP), button_pressed(BUTTON_JS_DOWN)
							,button_pressed(BUTTON_JS_LEFT), button_pressed(BUTTON_JS_RIGHT), button_pressed(BUTTON_JS_CENTER));
		
		led_control(LED_D2, LED_OFF);
		led_control(LED_D3, LED_OFF);
		
		if (button_pressed(BUTTON_JS_UP)) {
			led_control(LED_D2, LED_ON);
			servo_val = servo_val<SERVO_MAX?servo_val+1:SERVO_MAX;
		}
		
		if (button_pressed(BUTTON_JS_DOWN)){
			led_control(LED_D3, LED_ON);
			servo_val = servo_val>SERVO_MIN?servo_val-1:SERVO_MIN;
		}
		
		if (button_pressed(BUTTON_1)){
			while(button_pressed(BUTTON_1)){
				button_update();
			}
			led_control(LED_D2, LED_ON);
			led_control(LED_D3, LED_ON);
			servo_val = SERVO_MIN;
		}
		
		if (button_pressed(BUTTON_2)){
			while(button_pressed(BUTTON_2)){
				button_update();
			}
			led_control(LED_D2, LED_ON);
			led_control(LED_D3, LED_ON);
			servo_val = SERVO_MAX;
		}
		
		tft_prints(0, y_index++, "%d", servo_val);
		servo_control(SERVO1, servo_val);
		servo_control(SERVO2, servo_val);
		servo_control(SERVO3, servo_val);
		servo_control(SERVO4, servo_val);
		
		tft_update();
		
		led_control(LED_D1, get_seconds()%2);
		*/
	}
}
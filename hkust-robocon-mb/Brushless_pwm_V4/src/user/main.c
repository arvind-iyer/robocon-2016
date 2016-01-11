#include "main.h"

uint16_t servo_val = SERVO_MIN;

int main(void) {
	
	led_init();
	
	tft_easy_init();
	ticks_init();
	button_init();
	servo_init();
	
	led_control(LED_D1, LED_ON);
	
	while(1){
		button_update();

		tft_clear();
		if (button_pressed(BUTTON_JS_UP)){
			if (servo_val<SERVO_MAX){
				servo_val++;
				led_control(LED_D3, LED_OFF);
			}else{
				led_control(LED_D3, LED_ON);
			}
			led_control(LED_D1, LED_ON);
		}else{
			led_control(LED_D1, LED_OFF);
		}
		
		if (button_pressed(BUTTON_JS_DOWN)){
			if (servo_val>SERVO_MIN){
				servo_val--;
				led_control(LED_D3, LED_OFF);
			}else{
				led_control(LED_D3, LED_ON);
			}
			led_control(LED_D2, LED_ON);
		}else{
			led_control(LED_D2, LED_OFF);
		}
		
		tft_prints(0, 0, "%d", servo_val);
		tft_update();
	}
}
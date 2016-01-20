#include "main.h"

//uint16_t servo_val = SERVO_MIN;

int main(void) 
{
	button_init();
	servo_init();
	led_init();
	ticks_init();
	tft_easy_init();
	while(1){
		button_update();
		
		if (get_full_ticks()%200==0){
			led_blink(LED_D1);
		}
		
		if (button_pressed(BUTTON_1)){
			while(button_pressed(BUTTON_1)){
				button_update();
			}

			servo_control_all(SERVO_MIN);
			tft_clear();
			tft_prints(0, 0, "MIN");
			tft_update();
		}
		
		if (button_pressed(BUTTON_2)){
			while(button_pressed(BUTTON_2)){
				button_update();
			}
			servo_control_all(SERVO_MAX);
						tft_clear();
			tft_prints(0, 0, "MAX");
			tft_update();
		}
	}
}
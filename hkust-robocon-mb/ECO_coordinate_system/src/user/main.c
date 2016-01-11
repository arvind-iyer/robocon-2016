#include "main.h"

uint16_t servo_val = SERVO_MIN;

int main(void) {
	
	led_init();
	
	tft_easy_init();
	ticks_init();
	button_init();
	servo_init();
	
	led_control(LED_D1, LED_ON);
	
	u8 count = 0;
	
	while(1){
		button_update();

		if (get_ticks()%500 == 0){
			count++;
			tft_append_line("%d", count);
			tft_update();
		}
		

		
	}
}
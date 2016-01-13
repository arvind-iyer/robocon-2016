#include "main.h"

uint16_t servo_val = SERVO_MIN;

int main(void) {
	
	led_init();
	
	tft_easy_init();
	ticks_init();
	//button_init();
	servo_init();
	encoder_init();
	
	led_control(LED_D1, LED_ON);
	
	u32 count[2] = {0};
	u32 last[2] = {0};
	
	while(1){
		button_update();

		if (get_full_ticks()%200 == 0){
			led_blink(LED_D1 | LED_D2);
		}
		 
		tft_clear();
		if (last[0]!=get_count(ENCODER1)) count[0]++;
		if (last[1]!=get_count(ENCODER2)) count[1]++;
		
		last[0] = get_count(ENCODER1);
		last[1] = get_count(ENCODER2);
		tft_append_line("%d", get_count(ENCODER1));
		tft_append_line("%d", get_count(ENCODER2));
		tft_append_line("%d", count[0]);
		tft_append_line("%d", count[1]);
		tft_update();
		
	}
}
#include "main.h"

//uint16_t servo_val = SERVO_MIN;

int main(void) 
{
	servo_init();
	led_init();
	ticks_init();
	tft_easy_init();
	tft_put_logo(110, 90);
	while(1){
		
		if (get_full_ticks()%200==0){
			led_blink(LED_D1);
		}
	}
}
#include "main.h"

uint16_t servo_val = SERVO_MIN;
u8 i=0;

int main(void) 
{
	
	led_init();
	
	//led_control(LED_D1, 1);
	
	tft_init(2, BLACK, WHITE, BLUE);
	ticks_init();
	button_init();
	servo_init();
	linear_ccd_init();
	
	//led_control(LED_D1, LED_ON);
	
	while(1)
	{
		linear_ccd_read();
		tft_clear();
		
		for(int i=0;i<128;i++)
		{
			//tft_prints(0,0,"%d[!]", i);
			tft_put_pixel(i, linear_ccd_buffer1[i], WHITE);
			//tft_update();
		}
		
		tft_update();
		
		for(int i=0;i<128;i++)
		{
			tft_put_pixel(i, linear_ccd_buffer1[i], BLACK);
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
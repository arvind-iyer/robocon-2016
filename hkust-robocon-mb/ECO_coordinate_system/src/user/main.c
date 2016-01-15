#include "main.h"

uint16_t servo_val = SERVO_MED;
u8 happy_state = 0;

int main(void) {
	
	led_init();
	
	tft_easy_init();
	ticks_init();
	servo_init();
	encoder_init();
	button_init();
	
	led_control(LED_D1, LED_ON);
	
	while(1){
		button_update();
		encoder_update();
		
		if (get_full_ticks()%200 == 0){
			led_blink(LED_D1 | LED_D2);
		}
		
		if (button_pressed(BUTTON_JS_UP)){
			servo_val++;
		}else if(button_pressed(BUTTON_JS_DOWN)){
			servo_val--;
		}
		
		if (get_count(ENCODER1)>2400 && happy_state==0){
			servo_val = 1993;
			happy_state = 1;
		}else if (get_count(ENCODER2)>3810){
			servo_val = SERVO_MED;
			happy_state = 2;
		}
		
		servo_control(SERVO1, servo_val); 
		servo_control(SERVO2, servo_val); 
		servo_control(SERVO3, servo_val); 
		servo_control(SERVO4, servo_val); 
		
		tft_clear();

		tft_append_line("EL:%d ER:%d", get_count(ENCODER1), get_count(ENCODER2));
		tft_append_line("VL:%d VR:%d", get_vel(ENCODER1), get_vel(ENCODER2));
		
		tft_append_line("ED:%d VD:%d", get_count(ENCODER1)-get_count(ENCODER2), get_vel(ENCODER1)-get_vel(ENCODER2));
		tft_append_line("Servo:%d", servo_val);
		
		tft_update();
	}
}
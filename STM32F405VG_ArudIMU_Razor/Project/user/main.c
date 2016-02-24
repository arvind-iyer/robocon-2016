#include "main.h"
#include <string.h>

//Create 2 Servo Objects
TM_SERVO_t Servo1;

u32 last_loop_ticks = 0;
u32 last_long_loop_ticks = 0;
u32 this_loop_ticks = 0;
u32 last_short_loop_ticks = 0;

int main(void) {
	led_init();
	TM_DELAY_Init();
	TM_SERVO_Init(&Servo1, TIM3, TM_PWM_Channel_2, TM_PWM_PinsPack_1);
	imu_init();
	tft_easy_init(); //Init LCD
	tft_put_logo(85, 120);
	led_control(LED_D2, LED_ON);
	
	while (1) { 
		if(get_ticks() != this_loop_ticks){
			
			TM_SERVO_SetDegrees(&Servo1, 90);
			
			this_loop_ticks = get_ticks();
			uart_tx_byte(COM1, 64);
			//Long loop action
			if (this_loop_ticks - last_long_loop_ticks > LONG_LOOP_TICKS){
				led_blink(LED_D1);
				imu_update();
				last_long_loop_ticks = this_loop_ticks;
			}
			
			last_loop_ticks = this_loop_ticks;
		}
	}

}

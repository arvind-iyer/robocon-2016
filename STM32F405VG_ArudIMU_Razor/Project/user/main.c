#include "main.h"
#include <string.h>

//Create 2 Servo Objects
TM_SERVO_t Servo1,Servo2;

u32 last_loop_ticks = 0;
u32 last_long_loop_ticks = 0;
u32 this_loop_ticks = 0;
u32 last_short_loop_ticks = 0;

int main(void) {
	led_init();
	TM_DELAY_Init();
	//Initialize the 2 Servos' Pins (plz see TM_stm32f4_pwm.c for the channel and pinspack)
	TM_SERVO_Init(&Servo1, TIM4, TM_PWM_Channel_3, TM_PWM_PinsPack_1);
	TM_SERVO_Init(&Servo2, TIM4, TM_PWM_Channel_4, TM_PWM_PinsPack_1);
	imu_init();
	tft_easy_init(); //Init LCD
	tft_put_logo(85, 120);
	led_control(LED_D2, LED_ON);
    
	while (1) { 
		if(get_ticks() != this_loop_ticks){
			this_loop_ticks = get_ticks();
			
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

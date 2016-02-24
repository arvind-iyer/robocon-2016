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
	
	u8 game_stage = 0;
	u8 climbing_stage = 0;
	u8 climbing_count = 0;
	CLIMBING_STATE cimbing_state = ON_GROUND;
	TM_SERVO_SetDegrees(&Servo1, 90);
	
	while (1) { 
		if(get_ticks() != this_loop_ticks){
			
			this_loop_ticks = get_ticks();
			uart_tx_byte(COM1, 64);
			
			//Long loop action
			if (this_loop_ticks - last_long_loop_ticks > LONG_LOOP_TICKS){
				led_blink(LED_D1);
				imu_update();
				
				if (game_stage==0){
					if (yaw_pitch_roll[0] > 1500){
						if (cimbing_state == ON_GROUND){
							cimbing_state = ON_SLOPE;
							climbing_count = 0;
						}else{
							climbing_count++;
						}
					}else{
						if (cimbing_state == ON_SLOPE){
							cimbing_state = ON_GROUND;
							climbing_count = 0;
						}else{
							climbing_count++;
						}
					}
						
					if (climbing_count>5 && cimbing_state==ON_SLOPE){
						if (climbing_stage==1){
							TM_SERVO_SetDegrees(&Servo1, 110);
						}else if(climbing_stage==3){
							TM_SERVO_SetDegrees(&Servo1, 70);
						}
						climbing_stage++;
					}else if(climbing_count>5 && cimbing_state==ON_GROUND){
						if (climbing_stage==2){
							TM_SERVO_SetDegrees(&Servo1, 110);
						}else if(climbing_stage==4){
							TM_SERVO_SetDegrees(&Servo1, 70);
						}
						climbing_stage++;
					}
					
					if (climbing_stage>=6){
						game_stage = 1;
					}
				}else if(game_stage == 2){
					
				}
				
				last_long_loop_ticks = this_loop_ticks;
			}
			
			last_loop_ticks = this_loop_ticks;
		}
	}

}

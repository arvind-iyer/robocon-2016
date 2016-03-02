#include "main.h"

u32 last_loop_ticks = 0;
u32 last_long_loop_ticks = 0;
u32 this_loop_ticks = 0;
u32 last_short_loop_ticks = 0;

int main(void) {
	led_init();
	TM_DELAY_Init();
	imu_init();
	servo_init();
	tft_easy_init(); //Init LCD
	tft_put_logo(85, 120);
	led_control(LED_D2, LED_ON);
	
	GAME_STAGE game_stage = SYSTEM_WAITING;
	
	while (1) { 
		if(get_ticks() != this_loop_ticks){
			
			this_loop_ticks = get_ticks();
			
			//Long loop action
			if (this_loop_ticks - last_long_loop_ticks > LONG_LOOP_TICKS){
				led_blink(LED_D1);
				tft_clear();
				imu_update();
				path_up_update();
				
				if (game_stage == SYSTEM_WAITING){
					if (imu_synced && imu_staged){
						game_stage = CLIMBING_SLOPE;
						targeting_update(yaw_pitch_roll[0]);
					}else if(!imu_synced){
						tft_println("[Not synced]");
					}else if(!imu_staged){
						tft_println("[Not staged]");
					}
				}else{
					switch(game_stage){
						case CLIMBING_SLOPE:
							tft_println("[CLIMBING]");
							break;
						case CROSSING_RIVER:
							tft_println("[RIVERING]");
							break;
						case GOING_DOWN_HILL:
							tft_println("[GOING DOWN]");
							break;
						case WINNING_THE_GAME:
							tft_println("[WIN OR FUCK UP]");
							break;
						default:
							tft_println("[WTF MAN]");
					}
				}
				
				tft_println("Loop: %d %d", this_loop_ticks, this_loop_ticks-last_long_loop_ticks);
				tft_println("Yaw: %f", yaw_pitch_roll[0]);
				tft_println("Pit: %f", yaw_pitch_roll[1]);
				tft_println("Rol: %f", yaw_pitch_roll[2]);
				tft_update();
				last_long_loop_ticks = this_loop_ticks;
				
			}else if (this_loop_ticks - last_short_loop_ticks > SHORT_LOOP_TICKS){
				//Short loop action, does not run twice when long loop is also ran
				imu_update();
				path_up_update();
				if (imu_synced && imu_staged){
					targeting_update(yaw_pitch_roll[0]);
				}
				last_short_loop_ticks = this_loop_ticks;
			}
			
			last_loop_ticks = this_loop_ticks;
		}
	}

}

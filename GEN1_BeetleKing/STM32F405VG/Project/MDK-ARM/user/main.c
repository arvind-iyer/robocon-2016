#include "main.h"

u32 last_loop_ticks = 0;
u32 last_long_loop_ticks = 0;
u32 this_loop_ticks = 0;
u32 last_short_loop_ticks = 0;
u32 any_loop_diff = 0;

int main(void) {
	led_init();
	TM_DELAY_Init();
	imu_init();
	sensorbar_init();
	servo_init();
	tft_easy_init(); //Init LCD
	buzzer_init();
	tft_put_logo(85, 120);
	
	GAME_STAGE game_stage = SYSTEM_WAITING;
	
	while (1) { 
		if(get_ticks() != this_loop_ticks){
			this_loop_ticks = get_ticks();
			
			//Long loop action
			any_loop_diff = this_loop_ticks - last_long_loop_ticks;
			if (any_loop_diff > LONG_LOOP_TICKS){
				led_blink(LED_D1);
				tft_clear();
				imu_update();

				if (game_stage == SYSTEM_WAITING){
					if (imu_synced && imu_staged){
						//game_stage = CLIMBING_SLOPE;
						game_stage = GOING_DOWN_HILL;
						buzzer_play_song(SUCCESSFUL_SOUND, 100, 0);
						set_target(cal_ypr[0]);
					}else if(!imu_synced){
						tft_println("[Not synced]");
					}else if(!imu_staged){
						tft_println("[Not staged]");
					}
				}else{
					switch(game_stage){
						case CLIMBING_SLOPE:
							tft_println("[CLIMBING]");
							game_stage = path_up_update(); 
							break;
						case CROSSING_RIVER:
							tft_println("[RIVERING]");
							game_stage = path_river_update();
							break;
						case GOING_DOWN_HILL:
							tft_println("[GOING DOWN]");
							game_stage = path_down_update();
							break;
						case WINNING_THE_GAME:
							tft_println("[WIN OR FUCK UP]");
							break;
						default:
							tft_println("[WTF MAN]");
					}
				}
				
				tft_println("Loop: %d %d", this_loop_ticks, any_loop_diff);
				tft_println("%d %d %d", (int)roundf(cal_ypr[0]*10), (int)roundf(cal_ypr[1]*10), (int)roundf(cal_ypr[2]*10));
				for (u8 i=0; i<16; i++){
					//tft_prints(i, 8, "%d", sensorbar_value[i]);
				}
				tft_update();
				last_long_loop_ticks = this_loop_ticks;
			}
			last_loop_ticks = this_loop_ticks;
		}
	}

}

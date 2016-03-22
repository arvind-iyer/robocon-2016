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
	tft_easy_init((TFT_ORIENTATION)ORIENTATION_SETTING); //Init LCD
	buzzer_init();
	button_init();
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
				tft_println("[~BEETLE KING~]");
				imu_update();

				if (button_pressed(BUTTON_JS_CENTRE)){
					game_stage = IN_MENU;
					menu_init();
				}
				
				switch(game_stage){
					case IN_MENU:
						game_stage = menu_update();
						break;
					
					case SYSTEM_WAITING:
						if (imu_synced && imu_staged){
							game_stage++;
							buzzer_play_song(START_UP, 50, 0);
							set_target(cal_ypr[0]);
						}else if(!imu_synced){
							tft_println("[Not synced]");
						}else if(!imu_staged){
							tft_println("[Not staged]");
						}
						break;
						
					case SYSTEM_CALI:
						game_stage = imu_cali();
						if (game_stage != SYSTEM_CALI){
							buzzer_play_song(SUCCESSFUL_SOUND, 100, 0);
						}
						break;
						
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
					
					case PURE_SENSOR_BAR:
						sensor_bar_track();
						break;
					
					default:
						tft_println("[WTF]");
				}
				
				if (game_stage != IN_MENU){
					tft_println("Loop: %d %d", this_loop_ticks, any_loop_diff);
					tft_println("%d %d %d", (int)roundf(cal_ypr[0]*10), (int)roundf(cal_ypr[1]*10), (int)roundf(cal_ypr[2]*10));
					
					for (u8 i=0; i<16; i++){
						tft_prints(i, 8, "%d", sensorbar_value[i]);
					}
					tft_update();
				}
				last_long_loop_ticks = this_loop_ticks;
			}
			last_loop_ticks = this_loop_ticks;
		}
	}

}

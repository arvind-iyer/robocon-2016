#include "main.h"

u32 last_loop_ticks = 0;
u32 last_long_loop_ticks = 0;
u32 this_loop_ticks = 0;
u32 last_short_loop_ticks = 0;
u32 any_loop_diff = 0;

int main(void) {
	SystemInit();
	SystemCoreClockUpdate();
	ticks_init();
	led_init();
	sensorbar_init();
	servo_init();
	si_init();
	tft_easy_init((TFT_ORIENTATION)ORIENTATION_SETTING); //Init LCD
	buzzer_init();
	button_init();
	buzzer_play_song(SUCCESSFUL_SOUND, 100, 0);
	tft_put_logo(85, 120);
	ardu_adapter_init();
	
	GAME_STAGE game_stage = SYSTEM_WAITING;
	
	while (1) {
		this_loop_ticks = get_full_ticks();
		if(this_loop_ticks != last_loop_ticks){
			
			//Long loop action
			any_loop_diff = this_loop_ticks - last_long_loop_ticks;
			if (any_loop_diff > LONG_LOOP_TICKS){
				led_blink(LED_D1);
				tft_clear();
				tft_println("[~BEETLE KING~]");
				ardu_imu_update();

				if (button_pressed(BUTTON_PIN) && game_stage != IN_MENU){
					game_stage = IN_MENU;
					menu_init();
				}
				
				switch(game_stage){
					case IN_MENU:
						game_stage = menu_update();
						break;
					
					case SYSTEM_WAITING:
						if (ardu_all_ready()){
							game_stage++;
						}else{
							tft_println("[SYSTEM WAITING]");
						}
						break;
						
					case CLIMBING_SLOPE:
						tft_println("[CLIMBING]");
						#ifdef IMU_UPSLOPE
							game_stage = path_up_imu_update(); 
						#else
							game_stage = path_up_sb_update(); 
						#endif
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
						sensor_bar_track(PURE_SENSOR_BAR_POWER, PURE_SENSOR_BAR_Kp);
						break;
					
					default:
						tft_println("[WTF]");
				}
				
				if (game_stage != IN_MENU){
					tft_println("Loop: %d %d", this_loop_ticks, any_loop_diff);
					tft_println("%d %d %d", ardu_int_ypr[0], ardu_int_ypr[1], ardu_int_ypr[2]);
					
					for (u8 i=0; i<16; i++){
						tft_prints(i, 8, "%d", sensor_bar_value[i]);
					}
					tft_update();
				}
				last_long_loop_ticks = this_loop_ticks;
			}
			last_loop_ticks = this_loop_ticks;
		}
	}
}
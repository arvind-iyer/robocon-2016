#include "main.h"

u32 last_loop_ticks = 0;
u32 last_long_loop_ticks = 0;
u32 this_loop_ticks = 0;
u32 last_short_loop_ticks = 0;
u32 any_loop_diff = 0;
u8 short_loop_per_long_loop = 0;

int main(void) {
	SystemInit();
	SystemCoreClockUpdate();
	ticks_init();
	led_init();
	sensorbar_init();
	encoder_init();
	servo_init();
	si_init();
	#ifdef BLUE_FIELD
		tft_init((TFT_ORIENTATION)ORIENTATION_SETTING, BLACK, WHITE, BLUE); //Init LCD
	#else
		tft_init((TFT_ORIENTATION)ORIENTATION_SETTING, BLACK, WHITE, RED); //Init LCD
	#endif
	buzzer_init();
	button_init();
	buzzer_play_song(START_UP, 100, 0);
	tft_put_logo(85, 120);
	adc_init();
	
	mti_init();
	
	GAME_STAGE game_stage = SYSTEM_WAITING;

	while (1) {
		this_loop_ticks = get_full_ticks();
						
		//Long loop action
		any_loop_diff = this_loop_ticks - last_long_loop_ticks;
		if (any_loop_diff > LONG_LOOP_TICKS){
			led_blink(LED_D1);
			tft_clear();
			tft_println("[~MOBILE CASTLE~]");

			game_stage = menu_update(game_stage);
			
			switch(game_stage){
				case SYSTEM_WAITING:
					if (mti_all_ready()){
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
					//Update slower for down slope
					break;
				
				case IN_STATION:
					tft_println("[WIND STATION]");
					game_stage = path_station_update();
					break;
				
				case SENSOR_BAR_CALI:
					game_stage = sb_cali_update();
					break;
				
				default:
					tft_println("[WTF]");
			}
			
			tft_println("LP: %d %d", this_loop_ticks, any_loop_diff, short_loop_per_long_loop);
			tft_println("ANG:%d %d %d", mti_int_ypr[0], mti_int_ypr[1], mti_int_ypr[2]);
			tft_println("EN:%d %d", get_dis(ENCODER1), get_dis(ENCODER2));
			tft_println("CS:%d %d %d", get_x(), get_y(), get_z());
			tft_println("SR: %s", colors_string[sensorbar_region]);
			short_loop_per_long_loop = 0;
			
			for (u8 i=0; i<16; i++){
				tft_prints(i, 8, "%d", sensor_bar_filtered[i]);
			}
			tft_update();
			last_long_loop_ticks = this_loop_ticks;
			last_short_loop_ticks = this_loop_ticks;
		}else{
			//Short loop action
			any_loop_diff = this_loop_ticks - last_short_loop_ticks;
			if (any_loop_diff > SHORT_LOOP_TICKS){
				short_loop_per_long_loop++;
				mti_update();
				coord_update();
				get_count(ENCODER1);
				get_count(ENCODER2);
				switch(game_stage){
					case CLIMBING_SLOPE:
						#ifdef IMU_UPSLOPE
							game_stage = path_up_imu_update(); 
						#else
							game_stage = path_up_sb_update(); 
						#endif
						break;
					
					case CROSSING_RIVER:
						game_stage = path_river_update();
						break;
					
					case GOING_DOWN_HILL:
						game_stage = path_down_update();
						break;
					
					default:
						break;
				}
				last_short_loop_ticks = this_loop_ticks;
			}
		}
		last_loop_ticks = this_loop_ticks;
	}
}

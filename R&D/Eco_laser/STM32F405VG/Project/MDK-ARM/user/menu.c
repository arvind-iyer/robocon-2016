#include "menu.h"

static const char* items_in_menu[NUMBER_OF_ITEMS] = {"START ZONE", "HILL 1", "HILL 2", 
				"CROSS RIVER", "DOWN HILL", "STATION", "SENSOR CALI"};


s8 selection = 0;

void menu_init(){
	selection = 0;
}

GAME_STAGE menu_update(){
	tft_clear();
	for (u8 i=0;i<NUMBER_OF_ITEMS;i++){
		if (i == selection){
			tft_set_bg_color(RED);
			tft_set_text_color(WHITE);
		}else{
			tft_set_bg_color(WHITE);
			tft_set_text_color(BLACK);
		}
		tft_println(items_in_menu[i]);
	}
	tft_set_bg_color(WHITE);
	
	if (button_pressed(BUTTON_PIN)){
		u32 pressed_ticks = get_full_ticks();
		while(button_pressed(BUTTON_PIN));
		if (get_full_ticks()-pressed_ticks > 500){
			tft_set_bg_color(BLACK);
			tft_set_text_color(WHITE);
			switch (selection){
				case 0:
					#ifdef IMU_UPSLOPE
						path_up_imu_init(selection);
					#else
						path_up_sb_init(selection);
					#endif
					mti_int_ypr[0] = 0;
					return CLIMBING_SLOPE;
				case 1:
					#ifdef IMU_UPSLOPE
						path_up_imu_init(selection);
					#else
						path_up_sb_init(selection);
					#endif
					#ifdef BLUE_FIELD
						mti_int_ypr[0] = -300;
					#else
						mti_int_ypr[0] = 300;
					#endif
					buzzer_play_song(HIGH_1, 500, 50);
					return CLIMBING_SLOPE;
				case 2:
					#ifdef IMU_UPSLOPE
						path_up_imu_init(selection);
					#else
						path_up_sb_init(selection);
					#endif
					buzzer_play_song(HIGH_2, 300, 150);
					#ifdef BLUE_FIELD
						mti_int_ypr[0] = -450;
					#else
						mti_int_ypr[0] = 450;
					#endif
					return CLIMBING_SLOPE;
				case 3:
					#ifdef BLUE_FIELD
						mti_int_ypr[0] = 900;
					#else
						mti_int_ypr[0] = -900;
					#endif
					return CROSSING_RIVER;
				case 4:
					path_down_reset();
					buzzer_play_song(HIGH_4, 200, 100);
					return GOING_DOWN_HILL;
				case 5:
					buzzer_play_song(MARIO_BEGIN, 100, 0);
					path_station_reset();
					return IN_STATION;
				case 6:
					return SENSOR_BAR_CALI;
			}
			mti_immediate_ready();
		}else{
			selection = (selection+1) % NUMBER_OF_ITEMS;
		}
	}
	
	tft_update();
	return IN_MENU;
}

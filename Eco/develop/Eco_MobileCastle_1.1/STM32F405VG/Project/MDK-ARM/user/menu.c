#include "menu.h"

static const char* items_in_menu[NUMBER_OF_ITEMS] = {"START ZONE", "HILL 1", "HILL 2", 
				"CROSS RIVER", "DOWN_HILL", "PURE SENSROBAR", "SENSOR CALI"};


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
				case 1:
				case 2:
					#ifdef IMU_UPSLOPE
						path_up_imu_init(selection);
					#else
						path_up_sb_init(selection);
					#endif
					return CLIMBING_SLOPE;
				case 3:
					#ifdef BLUE_FIELD
						path_river_init(ardu_int_ypr[0]);
					#else
						path_river_init(ardu_int_ypr[0]);
					#endif
					return CROSSING_RIVER;
				case 4:
					path_down_init();
					return GOING_DOWN_HILL;
				case 5:
					return PURE_SENSOR_BAR;
				case 6:
					tft_println("HI THERE");
					buzzer_play_song(MARIO_BEGIN, 50, 0);
					return SYSTEM_WAITING;
				case 7:
					return SENSOR_BAR_CALI;
			}
		}else{
			selection = (selection+1) % NUMBER_OF_ITEMS;
		}
	}
	
	tft_update();
	return IN_MENU;
}

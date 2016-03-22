#include "menu.h"

static const char* items_in_menu[NUMBER_OF_ITEMS] = {"START ZONE", "HILL 1", "HILL 2", 
				"CROSS RIVER", "DOWN_HILL", "PURE SENSROBAR", "SAY HI"};

s8 selection = 0;

void menu_init(){
	selection = 0;
}

GAME_STAGE menu_update(){
	led_blink(LED_D4);
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
	
	if (button_pressed(BUTTON_JS_DOWN)){
		while(button_pressed(BUTTON_JS_DOWN));
		selection = (selection+1) % NUMBER_OF_ITEMS;
	}else if(button_pressed(BUTTON_JS_UP)){
		while(button_pressed(BUTTON_JS_UP));
		selection = selection<1?NUMBER_OF_ITEMS-1:selection-1;
	}
	
	if (button_pressed(BUTTON_JS_RIGHT)){
		while(button_pressed(BUTTON_JS_RIGHT));
		tft_set_bg_color(BLACK);
		tft_set_text_color(WHITE);
		switch (selection){
			case 0:
			case 1:
			case 2:
				path_up_init(selection);
				return CLIMBING_SLOPE;
			case 3:
				path_river_init();
				return CROSSING_RIVER;
			case 4:
				path_down_init();
				return GOING_DOWN_HILL;
			case 5:
				return PURE_SENSOR_BAR;
			case 6:
				tft_println("HI THERE");
				buzzer_play_song(MARIO_BEGIN, 50, 0);
				return SYSTEM_CALI;
		}
	}
	
	tft_update();
	return IN_MENU;
}

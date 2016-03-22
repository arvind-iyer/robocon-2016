#include "menu.h"

static const char* items_in_menu[NUMBER_OF_ITEMS] = {"START ZONE", "HILL 1", "HILL 2", 
				"CROSS RIVER", "DOWN_HILL", "PURE SENSROBAR", "SAY HI"};

u8 selection = 0;

void menu_init(){
	selection = 0;
}

void menu_update(){
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
	
	tft_update();
}

#include "sb_cali.h"

const char* colors_string[NUMBER_OF_COLOR] = {"RED START", "BLUE START", "SLOPE GREEN", 
			"HIGH ORANGE", "RIVER BLUE", "DOWN GREEN"};
				
u8 region_index = 0;
u8 cali_in_each_region = 0;
				
void sb_cali_init(){
	region_index = 0;
	cali_in_each_region = 0;
}
				
GAME_STAGE sb_cali_update(){
	si_clear();
	
	if (sensorbar_cali > 0){
		region_index = (sensorbar_cali-1) / 10;
		cali_in_each_region = (sensorbar_cali-1) % 10;
		
		tft_println(colors_string[region_index]);
		tft_println("REG: %d TIME: %d", region_index, cali_in_each_region);
	}else{
		tft_println("SCAN FULL WHITE");
	}
	tft_println("REPLACED: %d", received_color_count);
	
//	while(1){
//		tft_clear();
//		tft_println("%d", raw[0]);
//		tft_println("%d %d", raw[1], raw[2]);
//		tft_println("%d %d", raw[3], raw[4]);
//		tft_println("%d %d", raw[5], raw[6]);
//		tft_println("%d %d %d", raw[8], raw[9], raw[10]);
//		tft_update();
//	}
	
	si_execute();
	return SENSOR_BAR_CALI;
}

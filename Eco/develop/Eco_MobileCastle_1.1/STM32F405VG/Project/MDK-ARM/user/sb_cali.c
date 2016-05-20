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
	
	region_index = sensorbar_cali / 10;
	cali_in_each_region = sensorbar_cali % 10;
	
	tft_println(colors_string[region_index]);
	tft_println("REGION: %d", region_index);
	tft_println("TIMES: %d", cali_in_each_region);
	
	si_execute();
	return SENSOR_BAR_CALI;
}

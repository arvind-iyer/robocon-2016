#include "path_downslope.h"

static u8 down_state = 0;

void path_down_init(){
	down_state = 0;
}

u16 time_count = 0;

GAME_STAGE path_down_update(){
	tft_println("DOWN STATE: %d", down_state);
	sensor_bar_track(DOWN_SENSOR_BAR_POWER, DOWN_SENSOR_BAR_Kp);
	return GOING_DOWN_HILL;
}

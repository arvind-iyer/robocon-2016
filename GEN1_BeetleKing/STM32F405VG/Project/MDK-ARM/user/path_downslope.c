#include "path_downslope.h"

static float start_ypr[3];

void path_down_init(float* ypr){
	//Copy the starting yaw pitch roll to a private array
	memcpy(start_ypr, ypr, 3 * sizeof(float));
}

GAME_STAGE path_down_update(){
	return GOING_DOWN_HILL;
}

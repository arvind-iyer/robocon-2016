#include "path_downslope.h"

static u8 down_state = 0;

void path_down_init(){
	down_state = 0;
}

GAME_STAGE path_down_update(){
	tft_println("DOWN STATE: %d", down_state);
	switch (down_state){
		case 0:
			force_set_angle(SERVO_MED_DEG + 5.0f);
			if (cal_ypr[0] > 60.0f){
				down_state++;
			}
			break;
		case 1:
			force_set_angle(SERVO_MED_DEG + 30.0f);
			if (cal_ypr[0] > 90.0f){
				down_state++;
			}
		case 2:
			force_set_angle(SERVO_MED_DEG);
			if (cal_ypr[0] > 120.0f){
				down_state++;
			}
			break;
	}
	return GOING_DOWN_HILL;
}

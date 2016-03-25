#include "path_downslope.h"

static u8 down_state = 0;

void path_down_init(){
	down_state = 0;
}

u16 time_count = 0;

GAME_STAGE path_down_update(){
	tft_println("DOWN STATE: %d", down_state);
	sensor_bar_track(DOWN_SENSOR_BAR_POWER, DOWN_SENSOR_BAR_Kp);
	
//	switch(down_state){
//		case 0:
//			force_set_angle(SERVO_MED_DEG + 10.0f);
//			if (time_count>1500){
//				time_count = 0;
//				down_state++;
//			}
//			break;
//		case 1:
//			force_set_angle(SERVO_MED_DEG + 5.0f);
//			if (time_count>1){
//				time_count = 0;
//				down_state++;
//			}
//			break;
//		case 2:
//			force_set_angle(SERVO_MED_DEG);
//			if (time_count>650){
//				time_count = 0;
//				down_state++;
//			}
//			break;
//		case 3:
//			force_set_angle(SERVO_MED_DEG - 15.0f);
//			if (time_count>1500){
//				time_count = 0;
//				down_state++;
//			}
//			break;
//		case 4:
//			force_set_angle(SERVO_MED_DEG);
//			if (time_count>1500){
//				time_count = 0;
//				down_state++;
//			}
//			break;
//		case 5:
//			force_set_angle(SERVO_MED_DEG + 15);
//			if (time_count>500){
//				time_count = 0;
//				down_state++;
//			}
//			break;
//		case 6:
//			force_set_angle(SERVO_MED_DEG);
//			if (time_count>2500){
//				time_count = 0;
//				down_state++;
//			}
//			break;
//	}
	
//	switch (down_state){
//		#ifdef BLUE_FIELD
//		case 0:
//			force_set_angle(SERVO_MED_DEG + 5.0f);
//			if (cal_ypr[0] > 60.0f){
//				down_state++;
//			}
//			break;
//		case 1:
//			force_set_angle(SERVO_MED_DEG + 30.0f);
//			if (cal_ypr[0] > 90.0f){
//				down_state++;
//			}
//		case 2:
//			force_set_angle(SERVO_MED_DEG);
//			if (cal_ypr[0] > 120.0f){
//				down_state++;
//			}
//			break;
//		#else
//		case 0:
//			force_set_angle(SERVO_MED_DEG - 10.0f);
//			if (cal_ypr[0] < -60.0f){
//				down_state++;
//			}
//			break;
//		case 1:
//			force_set_angle(SERVO_MED_DEG - 30.0f);
//			if (cal_ypr[0] < -90.0f){
//				down_state++;
//			}
//		case 2:
//			force_set_angle(SERVO_MED_DEG);
//			if (cal_ypr[0] < -120.0f){
//				down_state++;
//			}
//			break;
//		#endif
//	}
	return GOING_DOWN_HILL;
}

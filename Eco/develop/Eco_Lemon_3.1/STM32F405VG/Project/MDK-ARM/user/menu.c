#include "menu.h"

GAME_STAGE menu_update(GAME_STAGE current){
	
	//Sensor bar calibration
	if (button_pressed(BUT_1)){
		while(button_pressed(BUT_1));
//		#ifdef IMU_UPSLOPE
//			path_up_imu_init(1);
//		#else
//			path_up_sb_init(1);
//		#endif
//		#ifdef BLUE_FIELD
//			mti_start_bias_yaw = -300;
//		#else
//			mti_start_bias_yaw = 300;
//		#endif
		buzzer_play_song(HIGH_1, 500, 50);
//		return CLIMBING_SLOPE;
		MTi_1_reset();
		return SENSOR_BAR_CALI;
	}
	
	//First Hill retry
	if (button_pressed(BUT_5)){
		while(button_pressed(BUT_5));
		#ifdef IMU_UPSLOPE
			path_up_imu_init(1);
		#else
			path_up_sb_init(1);
		#endif
		#ifdef BLUE_FIELD
			mti_start_bias_yaw = -300;
		#else
			mti_start_bias_yaw = 300;
		#endif
		buzzer_play_song(HIGH_1, 500, 50);
		MTi_1_reset();
		return CLIMBING_SLOPE;
	}
	
	//Second hill retry
	if (button_pressed(BUT_6)){
		while(button_pressed(BUT_6));
		#ifdef IMU_UPSLOPE
			path_up_imu_init(2);
		#else
			path_up_sb_init(2);
		#endif
		#ifdef BLUE_FIELD
			mti_start_bias_yaw = -300;
		#else
			mti_start_bias_yaw = 300;
		#endif
		buzzer_play_song(HIGH_2, 500, 50);
		MTi_1_reset();
		return CLIMBING_SLOPE;
	}
	
	//River retry
	if (button_pressed(BUT_7)){
		while(button_pressed(BUT_7));
		#ifdef BLUE_FIELD
			mti_start_bias_yaw = 900;
			path_river_init(900);
		#else
			mti_start_bias_yaw = -900;
			path_river_init(-900);
		#endif
		MTi_1_reset();
		return CROSSING_RIVER;
	}
	
	//Highland retry
	if (button_pressed(BUT_8)){
		while(button_pressed(BUT_8));
		path_down_reset();
		buzzer_play_song(HIGH_4, 200, 100);
		MTi_1_reset();
		return GOING_DOWN_HILL;
	}

	return current;
}

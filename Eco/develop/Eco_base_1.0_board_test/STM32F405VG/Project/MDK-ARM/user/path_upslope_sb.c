#include "path_upslope_imu.h"

GAME_STAGE path_up_sb_update(){	
	SENSOR_BAR_FLAG flag;
	s16 correction = sensor_bar_get_corr(UP_SENSOR_BAR_POWER, UP_SENSOR_BAR_Kp, &flag);
	
	if (flag == SENSOR_BAR_ALL){
		//When read the all white line, go to next stage
		return CLIMBING_SLOPE + 1;
	}
	
	si_clear();
	si_add_pwm_bias(correction);
	si_execute();
	
	tft_println("SF: %d %d", flag, correction);
	
	return CLIMBING_SLOPE;
}

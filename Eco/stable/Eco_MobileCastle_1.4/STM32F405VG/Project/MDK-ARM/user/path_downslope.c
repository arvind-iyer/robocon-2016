#include "path_downslope.h"

static s32 start_dis = 0;

void path_down_reset(){
	start_dis = get_average_dis();
}

GAME_STAGE path_down_update(){
	SENSOR_BAR_FLAG flag;
	
	sensor_bar_get_corr(1, 100, &flag);
	
	if (flag != SENSOR_BAR_NTH){
		u16 downslope_servo_pwm = sb_pwm_1to1(DOWN_SB_INC_PWM, DOWN_SB_DEC_PWM);
		
		si_clear();
		si_set_pwm(downslope_servo_pwm);
		si_execute();
	}
	
	if((get_average_dis() - start_dis > 800) && (flag == SENSOR_BAR_ALL || sensorbar_region == BLUE_START)){
		return (GAME_STAGE)(GOING_DOWN_HILL+1);
	}
	
	return GOING_DOWN_HILL;
}

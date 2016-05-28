#include "path_downslope.h"

void path_down_init(){
}

u16 time_count = 0;

GAME_STAGE path_down_update(){
	SENSOR_BAR_FLAG flag;
	
	sensor_bar_get_corr(1, 100, &flag);
	
	if (flag != SENSOR_BAR_NTH){
		u16 downslope_servo_pwm = SERVO_PROPER_MIN_PWM + (SERVO_PROPER_MAX_PWM-SERVO_PROPER_MIN_PWM)*sensor_bar_mid/10;
		
		si_clear();
		si_set_pwm(downslope_servo_pwm);
		si_execute();
	}
	
	if(flag == SENSOR_BAR_ALL || sensorbar_region == BLUE_START){
		return (GAME_STAGE)(GOING_DOWN_HILL+1);
	}
	
	return GOING_DOWN_HILL;
}

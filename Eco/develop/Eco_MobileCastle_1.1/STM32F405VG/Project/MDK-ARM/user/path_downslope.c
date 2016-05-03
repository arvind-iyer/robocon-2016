#include "path_downslope.h"

void path_down_init(){
}

u16 time_count = 0;

GAME_STAGE path_down_update(){
	SENSOR_BAR_FLAG flag;
	
	sensor_bar_get_corr(1, 100, &flag);
	
	if (flag != SENSOR_BAR_NTH){
		u16 downslope_servo_pwm = SERVO_MIN_PWM + (SERVO_MAX_PWM-SERVO_MIN_PWM)*sensor_bar_mid/10;
		
		si_clear();
		si_set_pwm(downslope_servo_pwm);
		si_execute();
	}
	
	return GOING_DOWN_HILL;
}

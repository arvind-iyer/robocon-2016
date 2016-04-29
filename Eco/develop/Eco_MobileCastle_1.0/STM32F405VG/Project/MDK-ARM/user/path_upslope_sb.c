#include "path_upslope_imu.h"

static u8 up_slope_line_count = 0;

void path_up_sb_init(u8 stage){
	up_slope_line_count = stage;
}

GAME_STAGE path_up_sb_update(){	
	SENSOR_BAR_FLAG flag;
	
	sensor_bar_get_corr(1, 100, &flag);
	
	if (flag == SENSOR_BAR_ALL){
		//When read the all white line, go to next stage
		return (GAME_STAGE)(CLIMBING_SLOPE + 1);
	}
	
	u16 upslope_servo_pwm = SERVO_MIN_PWM + (SERVO_MAX_PWM-SERVO_MIN_PWM)*sensor_bar_mid/16;
	
	si_clear();
	si_set_pwm(upslope_servo_pwm);
	si_execute();
	
	tft_println("SF: %d %d %d", flag, sensor_bar_mid, upslope_servo_pwm);
	
	return CLIMBING_SLOPE;
}

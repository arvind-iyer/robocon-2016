#include "path_upslope_imu.h"

static u8 right_angle_ing = 0;

void path_up_sb_init(u8 stage){
	right_angle_ing = 0;
}

GAME_STAGE path_up_sb_update(){	
	SENSOR_BAR_FLAG flag;
	
	sensor_bar_get_corr(1, 100, &flag);
	
	if (right_angle_ing == 1){
		if (abs(river_straight_yaw - ardu_int_ypr[0]) > 100){
			si_clear();
			#ifdef BLUE_FIELD
				si_set_pwm(SERVO_MAX_PWM);
			#else
				si_set_pwm(SERVO_MIN_PWM);
			#endif
			si_execute();
		}else{
			return (GAME_STAGE)(CLIMBING_SLOPE + 1);
		}
	}else{
	
		if (flag == SENSOR_BAR_ALL){
			right_angle_ing = 1;
		}
		
		u16 upslope_servo_pwm = SERVO_MIN_PWM + (SERVO_MAX_PWM-SERVO_MIN_PWM)*sensor_bar_mid/16;
		
		si_clear();
		si_set_pwm(upslope_servo_pwm);
		si_execute();
		
		tft_println("SF: %d %d", sensor_bar_mid, upslope_servo_pwm);
	}
	
	return CLIMBING_SLOPE;
}

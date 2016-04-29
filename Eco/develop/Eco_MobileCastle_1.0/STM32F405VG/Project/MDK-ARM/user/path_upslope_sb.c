#include "path_upslope_imu.h"

static u16 upslope_servo_pwm = SERVO_MED_PWM;

GAME_STAGE path_up_sb_update(){	
	SENSOR_BAR_FLAG flag;
	
	if (flag == SENSOR_BAR_ALL){
		//When read the all white line, go to next stage
		return (GAME_STAGE)(CLIMBING_SLOPE + 1);
	}
	
	s16 correction = 0;
	s16 get_corr_mono = sensor_bar_get_corr(1, 100, &flag);
	switch(abs(get_corr_mono)){
		case 0:
			break;
		case 1:
			correction = 130;
			break;
		case 2:
			correction = 200;
			break;
		case 3:
			correction = 360;
			break;
		case 4:
			correction = 450;
			break;
		case 5:
			correction = 510;
			break;
		case 6:
			correction = 650;
			break;
		case 7:
			correction = 780;
			break;
		case 8:
			correction = 1100;
			break;
	}
	correction = get_corr_mono<0?-correction:correction;
	upslope_servo_pwm += correction;
	upslope_servo_pwm = u16_cap(upslope_servo_pwm, SERVO_MAX_PWM, SERVO_MIN_PWM);
	
	si_clear();
	si_set_pwm(upslope_servo_pwm);
	si_execute();
	
	tft_println("SF: %d %d %d", flag, get_corr_mono, upslope_servo_pwm);
	
	return CLIMBING_SLOPE;
}

#include "path_upslope_imu.h"

static u16 upslope_servo_pwm = SERVO_MED_PWM;

GAME_STAGE path_up_sb_update(){	
	SENSOR_BAR_FLAG flag;
	
	if (flag == SENSOR_BAR_ALL){
		//When read the all white line, go to next stage
		return (GAME_STAGE)(CLIMBING_SLOPE + 1);
	}
	
	s16 correction = 0;
	s16 get_corr_mono = sensor_bar_get_corr_nf(1, 100);
	switch(abs(get_corr_mono)){
		case 0:
		case 1:
			break;
		case 2:
			correction = 230;
			break;
		case 3:
			correction = 320;
			break;
		case 4:
			correction = 470;
			break;
		case 5:
			correction = 620;
			break;
		case 6:
			correction = 740;
			break;
		case 7:
			correction = 800;
			break;
		case 8:
			correction = 900;
			break;
	}
	correction = get_corr_mono<0?-correction:correction;
	upslope_servo_pwm += correction;
	
	si_clear();
	si_set_pwm(upslope_servo_pwm);
	si_execute();
	
	tft_println("SF: %d %d", flag, upslope_servo_pwm);
	
	return CLIMBING_SLOPE;
}

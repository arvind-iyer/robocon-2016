#include "path_station.h"

static s32 last_encoder_count = 0;
static bool stopped = false;

void path_station_reset(){
	last_encoder_count = 0;
	stopped = false;
}

GAME_STAGE path_station_update(){
	
	if ((get_average_encoder() - last_encoder_count) > STOPPED_RUNNING_TOL && !stopped){
		SENSOR_BAR_FLAG flag;
		sensor_bar_get_corr(1, 100, &flag);
		if (flag != SENSOR_BAR_NTH){
			u16 downslope_servo_pwm = sb_pwm_1to1(DOWN_SB_INC_PWM, DOWN_SB_DEC_PWM);
			
			si_clear();
			si_set_pwm(downslope_servo_pwm);
			si_execute();
		}
		last_encoder_count = get_average_encoder();
		tft_println("WAITING");
	}else{
		si_clear();
		#ifdef BLUE_FIELD
			si_set_pwm(SERVO_PROPER_MIN_PWM);
		#else
			si_set_pwm(SERVO_PROPER_MAX_PWM);
		#endif
		si_execute();
		tft_println("STOPPED");
		stopped = true;
	}
	
	return IN_STATION;
}

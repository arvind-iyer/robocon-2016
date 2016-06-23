#include "path_downslope.h"

static s32 start_dis = 0;
static u16 last_pwm = SERVO_MED_PWM;

void path_down_reset(){
	start_dis = get_average_dis();
	last_pwm = SERVO_MED_PWM;
}

GAME_STAGE path_down_update(){
	SENSOR_BAR_FLAG flag;
	
	sensor_bar_get_corr(1, 100, &flag);
	
	if (flag != SENSOR_BAR_NTH){
		u16 downslope_servo_pwm = 0;
		if (sensorbar_region != HIGH_ORANGE){
			si_clear_static();
			downslope_servo_pwm = sb_pwm_1to1(DOWN_SB_INC_PWM, DOWN_SB_DEC_PWM, 0);
		}else{
			#ifdef BLUE_FIELD
				downslope_servo_pwm = sb_pwm_1to1(DOWN_SB_INC_PWM_AFTER_THIRD, DOWN_SB_DEC_PWM_AFTER_THIRD, SB_SHIFT_AFTER_THIRD);
			#else
				downslope_servo_pwm = sb_pwm_1to1(DOWN_SB_INC_PWM_AFTER_THIRD, DOWN_SB_DEC_PWM_AFTER_THIRD, -SB_SHIFT_AFTER_THIRD);
			#endif
		}
		
		if (abs((s32)downslope_servo_pwm-(s32)last_pwm)>500){
			last_pwm = downslope_servo_pwm;
			si_clear();
			si_set_pwm(downslope_servo_pwm);
			si_execute();
		}
	}
	
	tft_println("SE: %d", start_dis);
	
	#ifdef BLUE_FIELD
		if((get_average_dis() - start_dis > DOWN_SLOPE_MIN_DIS) && (flag == SENSOR_BAR_ALL || sensorbar_region == BLUE_START)){
			return (GAME_STAGE)(GOING_DOWN_HILL+1);
		}
	#else
		if((get_average_dis() - start_dis > DOWN_SLOPE_MIN_DIS) && (flag == SENSOR_BAR_ALL || sensorbar_region == RED_START)){
			return (GAME_STAGE)(GOING_DOWN_HILL+1);
		}
	#endif
	
	return GOING_DOWN_HILL;
}

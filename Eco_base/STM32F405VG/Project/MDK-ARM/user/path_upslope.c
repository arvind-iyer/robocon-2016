#include "path_upslope.h"

//For going up slope
static s16 rolling_pitch[ROLLING_PITCH_SIZE] = {0.0f};
static u8 rolling_pitch_index = 0;
static u16 progress_ticks = 0;
static s16 targeting_yaw;
static s16 awaiting_pitch;
static s16 last_pitch;
static bool using_sensor_bar = false;
#define TOTAL_PATH_SIZE 7

#ifdef BLUE_FIELD
	//This array marks the CHANGE in yaw, with repesct to the last value
	static const s16 path_yaw_change[TOTAL_PATH_SIZE] = {0, 270, 270, 270, -350, -300, -1200};
	//This array marks the pitch of the slope, with repesct to starting pitch
	static const s16 path_pitch_change[TOTAL_PATH_SIZE] = {0, -60, 0, -60, 0, -60, 0};
#else
	//This array marks the CHANGE in yaw, with repesct to the last value
	static const s16 path_yaw_change[TOTAL_PATH_SIZE] = {0, -30, -30, -30, 30, 30, 1200};
	//This array marks the pitch of the slope, with repesct to starting pitch
	static const s16 path_pitch_change[TOTAL_PATH_SIZE] = {0, -60, 0, -60, 0, -60, 0};
#endif

static uint8_t path_pointer = 0;
static s16 pitch_change = 0;
	
void path_up_init(u8 stage){
	path_pointer = rolling_pitch_index = stage*2;
	progress_ticks = 0;
	
	for (uint8_t i=0;i<ROLLING_PITCH_SIZE;i++){
		rolling_pitch[i] = ardu_start_ypr[1];
	}
	targeting_yaw = ardu_start_ypr[0];
	awaiting_pitch = last_pitch = ardu_start_ypr[1];
	targeting_yaw += path_yaw_change[path_pointer++];
	awaiting_pitch += path_pitch_change[path_pointer];
	pitch_change = abs_diff(last_pitch, awaiting_pitch);
}

GAME_STAGE path_up_update(){
	//Going up slope
	//Update the rolling average of pitch value
	rolling_pitch[rolling_pitch_index++] = ardu_cal_ypr[1];
	rolling_pitch_index = rolling_pitch_index % ROLLING_PITCH_SIZE;
	
	s16 median_pitch = get_median_of_five(rolling_pitch[0], rolling_pitch[1], rolling_pitch[2], rolling_pitch[3], rolling_pitch[4]);
	
	s16 curr_pitch_diff = abs_diff(median_pitch, awaiting_pitch);
	
	if (pitch_change > 0){
		//If pitch suppose to go up
		if (curr_pitch_diff < 1){
			progress_ticks += any_loop_diff;
		}else{
			progress_ticks = 0;
		}
	}else{
		//If pitch suppose to go down
		if (curr_pitch_diff > -1){
			progress_ticks += any_loop_diff;
		}else{
			progress_ticks = 0;
		}
	}
	
	if (progress_ticks > PROGRESS_TICKS_MAX){
		progress_ticks = 0;
		//progress when it is the right time
		targeting_yaw += path_yaw_change[path_pointer++];
		last_pitch = awaiting_pitch;
		awaiting_pitch = ardu_start_ypr[1] + path_pitch_change[path_pointer];
		pitch_change = abs_diff(last_pitch, awaiting_pitch);
		
		set_target(targeting_yaw);
		if (path_pointer >= TOTAL_PATH_SIZE){
			return (GAME_STAGE) (CLIMBING_SLOPE + 1);
		}
		led_blink(LED_D3);
	}
	
	SENSOR_BAR_FLAG flag;
	s16 correction = sensor_bar_get_corr(UP_SENSOR_BAR_POWER, UP_SENSOR_BAR_Kp, &flag);
	if (!using_sensor_bar && abs(correction) > UP_SENSOR_BAR_ON){
		using_sensor_bar = true;
	}else if(using_sensor_bar && abs(correction) < UP_SENSOR_BAR_OFF){
		using_sensor_bar = false;
	}
	
	si_clear();
	si_add_pwm_bias(targeting_pid(ardu_int_ypr[0]));
	tft_println("%d", targeting_pid(ardu_int_ypr[0]));
	if (using_sensor_bar){
		si_add_pwm_bias(correction);
	}
	si_execute();
	tft_println("PP:%d ST:%d", path_pointer, using_sensor_bar?1:0);
//	tft_println("AP:%f", awaiting_pitch);
//	tft_println("AV:%f", median_pitch);
	return CLIMBING_SLOPE;
}

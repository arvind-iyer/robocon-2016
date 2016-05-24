#include "path_upslope_imu.h"

//For going up slope
static s16 rolling_pitch[ROLLING_PITCH_SIZE] = {0.0f};
static u8 rolling_pitch_index = 0;
static u16 progress_ticks = 0;
static s16 targeting_yaw;
static s16 awaiting_pitch;
static s16 last_pitch;
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
	
void path_up_imu_init(u8 stage){
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

GAME_STAGE path_up_imu_update(){
	//Going up slope
	//Update the rolling average of pitch value
	rolling_pitch[rolling_pitch_index++] = ardu_int_ypr[1];
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
		awaiting_pitch = ardu_start_ypr[1]*10 + path_pitch_change[path_pointer];
		pitch_change = abs_diff(last_pitch, awaiting_pitch);
		
		set_target(targeting_yaw);
		if (path_pointer >= TOTAL_PATH_SIZE){
			return (GAME_STAGE) (CLIMBING_SLOPE + 1);
		}
	}
	
	SENSOR_BAR_FLAG flag;
	s16 correction = sensor_bar_get_corr(UP_SENSOR_BAR_POWER, UP_SENSOR_BAR_Kp, &flag);

	u16 sensorbar_trust = 0; //Scaled by 100
	switch(abs((s8)sensor_bar_mid - SENSOR_BAR_MID)){
		case 0:
		case 1:
			sensorbar_trust = 0;
			break;
		case 2:
			sensorbar_trust = 5;
			break;
		case 3:
			sensorbar_trust = 10;
			break;
		case 4:
			sensorbar_trust = 15;
			break;
		case 5:
			sensorbar_trust = 35;
			break;
		case 6:
			sensorbar_trust = 60;
			break;
		case 7:
			sensorbar_trust = 80;
			break;
		case 8:
			sensorbar_trust = 100;
			break;
	}
	
	si_clear();
	si_add_deg_bias(targeting_pid(ardu_int_ypr[0]));
	si_add_pwm_bias(correction*sensorbar_trust/100);
	si_execute();
	tft_println("PP:%d ST:%d MD:%d", path_pointer, sensorbar_trust, abs((s8)sensor_bar_mid - SENSOR_BAR_MID));
//	tft_println("AP:%d", awaiting_pitch);
//	tft_println("AV:%d", median_pitch);
	return CLIMBING_SLOPE;
}

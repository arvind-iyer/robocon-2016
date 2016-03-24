#include "path_upslope.h"

//For going up slope
static float rolling_pitch[ROLLING_PITCH_SIZE] = {0.0f};
static uint8_t rolling_pitch_index = 0;
static uint16_t progress_ticks = 0;
static float targeting_yaw;
static float awaiting_pitch;
static float last_pitch;
#define TOTAL_PATH_SIZE 7

#ifdef BLUE_FIELD
	//This array marks the CHANGE in yaw, with repesct to the last value
	static const float path_yaw_change[TOTAL_PATH_SIZE] = {0.0f, 27.0f, 27.0f, 27.0f, -35.0f, -30.0f, -120.0f};
	//This array marks the pitch of the slope, with repesct to starting pitch
	static const float path_pitch_change[TOTAL_PATH_SIZE] = {0.0f, -6.0f, 0.0f, -6.0f, 0.0f, -6.0f, 0.0f};
#else
	//This array marks the CHANGE in yaw, with repesct to the last value
	static const float path_yaw_change[TOTAL_PATH_SIZE] = {0.0f, -30.0f, -30.0f, -30.0f, 30.0f, 30.0f, 120.0f};
	//This array marks the pitch of the slope, with repesct to starting pitch
	static const float path_pitch_change[TOTAL_PATH_SIZE] = {0.0f, -6.0f, 0.0f, -6.0f, 0.0f, -6.0f, 0.0f};
#endif

static uint8_t path_pointer = 0;
static float pitch_change = 0;
	
void path_up_init(u8 stage){
	path_pointer = rolling_pitch_index = stage*2;
	progress_ticks = 0;
	
	for (uint8_t i=0;i<ROLLING_PITCH_SIZE;i++){
		rolling_pitch[i] = start_ypr[1];
	}
	targeting_yaw = start_ypr[0];
	awaiting_pitch = last_pitch = start_ypr[1];
	targeting_yaw += path_yaw_change[path_pointer++];
	awaiting_pitch += path_pitch_change[path_pointer];
	pitch_change = abs_diff(last_pitch, awaiting_pitch);
	enable_sensor_bar(UP_SLOPE_SENSOR_BAR_TRUST, 3); //Use the third power
}

GAME_STAGE path_up_update(){
	//Going up slope
	//Update the rolling average of pitch value
	rolling_pitch[rolling_pitch_index++] = cal_ypr[1];
	rolling_pitch_index = rolling_pitch_index % ROLLING_PITCH_SIZE;
	
	float median_pitch = get_median_of_five(rolling_pitch[0], rolling_pitch[1], rolling_pitch[2], rolling_pitch[3], rolling_pitch[4]);
	
	float curr_pitch_diff = abs_diff(median_pitch, awaiting_pitch);
	
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
		awaiting_pitch = start_ypr[1] + path_pitch_change[path_pointer];
		pitch_change = abs_diff(last_pitch, awaiting_pitch);
		
		set_target(targeting_yaw);
		if (path_pointer >= TOTAL_PATH_SIZE){
			disable_sensor_bar();
			return (GAME_STAGE) (CLIMBING_SLOPE + 1);
		}
		led_blink(LED_D3);
	}
	
	targeting_update(cal_ypr[0]);
	tft_println("PP:%d", path_pointer);
//	tft_println("AP:%f", awaiting_pitch);
//	tft_println("AV:%f", median_pitch);
	return CLIMBING_SLOPE;
}

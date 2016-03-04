#include "path_upslope.h"

static float start_ypr[3];

//For going up slope
static float rolling_pitch[ROLLING_PITCH_SIZE] = {0.0f};
static uint8_t rolling_pitch_index = 0;
static uint16_t progress_ticks = 0;
static float targeting_yaw;
static float awaiting_pitch;
static float last_pitch;
#define TOTAL_PATH_SIZE 7
//This mark the increment and decrement of yaw and pitch
static float path_yaw_change[TOTAL_PATH_SIZE] = {0.0f, 0.0f, 20.0f, 0.0f, -25.0f, 0.0f, -150.0f};
static float path_pitch_change[TOTAL_PATH_SIZE] = {0.0f, -5.0f, 5.0f, -5.0f, 5.0f, -5.0f, 5.0f};
static uint8_t path_pointer = 0;
	
void path_up_init(float* ypr){
	//Copy the starting yaw pitch roll to a private array
	memcpy(start_ypr, ypr, 3 * sizeof(float));
	path_pointer = rolling_pitch_index = progress_ticks = 0;
	
	for (uint8_t i=0;i<ROLLING_PITCH_SIZE;i++){
		rolling_pitch[i] = start_ypr[1];
	}
	targeting_yaw = start_ypr[0];
	awaiting_pitch = last_pitch = start_ypr[1];
	targeting_yaw += path_yaw_change[path_pointer++];
	awaiting_pitch += path_pitch_change[path_pointer];
}

GAME_STAGE path_up_update(){
	//Going up slope
	//Update the rolling average of pitch value
	rolling_pitch[rolling_pitch_index++] = yaw_pitch_roll[1];
	rolling_pitch_index = rolling_pitch_index % ROLLING_PITCH_SIZE;
	
	//Find the rolling average
	float average_pitch = 0.0f;
	for (uint8_t i=0; i<ROLLING_PITCH_SIZE; i++){
		average_pitch += rolling_pitch[i];
	}
	average_pitch /= ROLLING_PITCH_SIZE;
	
	float pitch_change = awaiting_pitch - last_pitch;
	pitch_change = pitch_change>180.0f ? pitch_change-360.0f : (pitch_change<-180.0f ? -(pitch_change+360.0f) : pitch_change);
	
	float curr_pitch_diff = average_pitch - awaiting_pitch;
	curr_pitch_diff = curr_pitch_diff>180.0f ? curr_pitch_diff-360.0f : (curr_pitch_diff<-180.0f ? -(curr_pitch_diff+360.0f) : curr_pitch_diff);
	
	if (pitch_change > 0){
		//If pitch suppose to go up
		if (curr_pitch_diff > 0){
			progress_ticks += this_loop_ticks - last_long_loop_ticks;
		}else{
			progress_ticks = 0;
		}
	}else{
		//If pitch suppose to go down
		if (curr_pitch_diff < 0){
			progress_ticks += this_loop_ticks - last_long_loop_ticks;
		}else{
			progress_ticks = 0;
		}
	}
	
	if (progress_ticks > PROGRESS_TICKS_MAX){
		progress_ticks = 0;
		//progress when it is the right time
		targeting_yaw += path_yaw_change[path_pointer++];
		last_pitch = awaiting_pitch;
		awaiting_pitch += path_pitch_change[path_pointer];
		set_target(targeting_yaw);
		if (path_pointer >= TOTAL_PATH_SIZE){
			return (GAME_STAGE) (CLIMBING_SLOPE + 1);
		}
	}
	tft_println("PP:%d AG:%d", path_pointer, (int)(average_pitch*10));
	tft_println("PT:%d LP:%d AP:%d", progress_ticks, (int)(last_pitch*10), (int)(awaiting_pitch*10));
	return CLIMBING_SLOPE;
}

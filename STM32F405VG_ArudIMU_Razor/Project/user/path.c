#include "path.h"

static float start_ypr[3];

static float path_yaw[] = {0.0, 15.0, 15.0};
static float path_pitch[] = {0.0, -6.5, 0.0};
static uint8_t path_pointer = 0;

static float rolling_pitch[ROLLING_PITCH_SIZE] = {0};
static uint8_t rolling_pitch_index = 0;
static uint8_t progress_count = 0;

static uint8_t current_stage = 0;
static float targeting_yaw;
static float awaiting_pitch;
	
void path_init(float* ypr){
	//Copy the starting yaw pitch roll to a private array
	memcpy(start_ypr, ypr, 3 * sizeof(float));
	path_pointer = current_stage = rolling_pitch_index = progress_count = 0;
	for (uint8_t i=0;i<ROLLING_PITCH_SIZE;i++){
		rolling_pitch[i] = start_ypr[1];
	}
	targeting_yaw = start_ypr[0];
	awaiting_pitch = start_ypr[1];
	targeting_yaw += path_yaw[path_pointer]++;
	awaiting_pitch += path_pitch[path_pointer];
}

void path_update(){
	//Update the rolling average of pitch value
	rolling_pitch[rolling_pitch_index++] = yaw_pitch_roll[1];
	rolling_pitch_index = rolling_pitch_index % ROLLING_PITCH_SIZE;
	
	//Find the rolling average
	float average_pitch = 0;
	for (uint8_t i=0; i<ROLLING_PITCH_SIZE; i++){
		average_pitch += rolling_pitch[i];
	}
	average_pitch /= ROLLING_PITCH_SIZE;
	
	if (awaiting_pitch > path_pitch[path_pointer-1]){
		//If pitch suppose to go up
		if (awaiting_pitch > average_pitch){
			progress_count++;
		}else{
			progress_count = 0;
		}
	}else{
		//If pitch suppose to go down
		if (awaiting_pitch < average_pitch){
			progress_count++;
		}else{
			progress_count = 0;
		}
	}
	
	if (progress_count>PROGRESS_COUNT_MAX){
		//progress when it is the right time
		targeting_yaw += path_yaw[path_pointer++];
		awaiting_pitch += path_pitch[path_pointer];
	}
}

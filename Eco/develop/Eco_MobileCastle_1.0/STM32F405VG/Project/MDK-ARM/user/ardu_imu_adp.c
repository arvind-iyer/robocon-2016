#include "ardu_imu_adp.h"

float ardu_start_ypr[3];
s16 ardu_int_ypr[3];

static bool sync_music_played = false;
static bool cali_music_played = false;

void ardu_adapter_init(){
	ardu_imu_init();
}

//A flag to indicate whether everything is ready and the robot is ready to run
bool ardu_all_ready(){
	return ardu_imu_staged && ardu_imu_calibrated;
}

//To be called ~20ms, handle most of the things
void ardu_imu_update(){
	ardu_imu_value_update();
	
	for (u8 i=0;i<3;i++){
		ardu_int_ypr[i] = (s16) roundf(ardu_cal_ypr[i]*10);
	}
	
	if (ardu_imu_pre_staged && !ardu_imu_staged){
		//This part of code should only run once asap after pre staged(set after syncing)
		set_target(ardu_cal_ypr[0]);
		//Copy the starting yaw pitch roll to a private array
		memcpy(ardu_start_ypr, ardu_cal_ypr, 3 * sizeof(float));
		
		#ifdef IMU_UPSLOPE
			path_up_imu_init(0);
		#else
			path_up_sb_init(0);
		#endif
		
		#ifdef BLUE_FIELD
			path_river_init(ardu_int_ypr[0] - 900);
		#else
			path_river_init(ardu_int_ypr[0] + 900);
		#endif
		path_down_init();
		ardu_imu_staged = true;
		//led_control(LED_D2, LED_ON);
	}
	
	if (!sync_music_played && ardu_imu_synced){
		buzzer_play_song(START_UP, 50, 0);
		sync_music_played = true;
	}
	
	if (!cali_music_played && ardu_imu_calibrated){
		buzzer_play_song(SUCCESSFUL_SOUND, 100, 0);
		cali_music_played = true;
	}
	
}

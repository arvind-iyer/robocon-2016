#include "imu_targeting.h"

s16 target_yaw;

void set_target(s16 in_target_yaw){
	target_yaw = in_target_yaw;
}

s16 targeting_pid(s16 current_yaw){
	//Find the diffence in yaw
	//float yaw_diff = abs_diff(target_yaw, current_yaw);
	s16 yaw_diff = current_yaw - target_yaw;
	tft_println("%d", yaw_diff);
	return yaw_diff;
}

void targeting_update(s16 current_yaw){
	si_add_deg_bias(targeting_pid(current_yaw));
}

#include "imu_targeting.h"

s16 target_yaw;
u16 current_pwm = 0;

void set_target(s16 in_target_yaw){
	target_yaw = in_target_yaw;
}

s16 targeting_pid(s16 current_yaw){
	//Find the diffence in yaw
	//float yaw_diff = abs_diff(target_yaw, current_yaw);
	s16 yaw_diff = target_yaw - current_yaw;
	
	//tft_println("TY:%d SV:%d", (int)(target_yaw*10),  current_pwm);

	return u16_sqrt((u16)abs(yaw_diff)) * SERVO_Kp * (yaw_diff>0?1:-1) / 100;
}

void targeting_update(s16 current_yaw){
	si_add_pwm_bias(targeting_pid(current_yaw));
}

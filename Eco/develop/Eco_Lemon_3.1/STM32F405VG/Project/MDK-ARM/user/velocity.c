/**
** Uses complementary filter to combine acceleration and encoder readings into velocity
** Rex Cheng
**/

#include "velocity.h"

float curr_vel = 0.0f; //In mm/s
s32 last_encoder_dis = 0;

void vel_update(){
	s32 curr_dis = get_average_dis();
	curr_vel += VEL_ACCEL_TRUST*(curr_vel + vector_len(MTi_acc)*1000.0f*1000.0f) /1000.0f +
								(1000-VEL_ACCEL_TRUST) * (curr_dis - last_encoder_dis) / (float)any_loop_diff;
	last_encoder_dis = curr_dis;
}

s32 get_vel(){
	return (s32)roundf(curr_vel);
}

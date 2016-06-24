#include "manual_track.h"

s16 curr_vx = 0, curr_vy = 0, curr_w = 0;
s16 motor_vel[3] = {0};
CLOSE_LOOP_FLAG motor_loop_state[3] = {CLOSE_LOOP};
static s16 accel_remain[3] = {0}; //vx, vy, w

//Factor scaled by 1000
static s16 get_new_v(s16 curr_v, s16 target_v, s16 accel_factor, s16 decel_factor, u16 scale, u16 cap, s16 *remainder){
	s32 accel_amount = *remainder;
	s16 v_diff = target_v - curr_v;
	
	if ((target_v>0 && curr_v>0)){
		if (target_v - curr_v > 0){
			accel_amount += (s32)accel_factor * (v_diff<cap?cap:v_diff) /scale;
		}else{
			accel_amount += (s32)decel_factor * (v_diff>-cap?-cap:v_diff) /scale;
		}
		
	}else if((target_v<0 && curr_v<0)){
		if (target_v - curr_v < 0){
			accel_amount += (s32)accel_factor * (v_diff>-cap?-cap:v_diff) /scale;
		}else{
			accel_amount += (s32)decel_factor * (v_diff<cap?cap:v_diff) /scale;
		}
		
	}else{
		if (v_diff>0 && v_diff<cap){
			v_diff = cap;
		}else if(v_diff<0 && v_diff>-cap){
			v_diff = -cap;
		}
		accel_amount += (s32)decel_factor * (v_diff) /scale;
	}
	
	if (abs(v_diff) <= (abs(accel_amount)+1)){
		*remainder = 0;
		return target_v;
	}else{
		//*remainder = accel_amount%1000;
		return curr_v + accel_amount;
	}
}

//Update in local coordinate
//All acceleration scaled by 1000
void acc_update(s16 vx, s16 vy, s16 w, s16 vx_acc, s16 vx_dec, s16 vy_acc, s16 vy_dec, s16 w_acc, s16 w_dec, bool global){
	
	curr_vx = get_new_v(curr_vx, vx, vx_acc, vx_dec, 1000, 400,  &accel_remain[0]);
	curr_vy = get_new_v(curr_vy, vy, vy_acc, vy_dec, 1000, 400,  &accel_remain[1]);
	curr_w = get_new_v(curr_w, w, w_acc, w_dec, 100, 8, &accel_remain[2]);

	s16 curr_angle = int_arc_tan2(curr_vx, curr_vy)*10;
	u32 curr_speed = u32_sqrt(curr_vy * curr_vy + curr_vx * curr_vx);
	motor_vel[0] = (int_sin(curr_angle%3600)*(s32)curr_speed*(-1)/10000 + curr_w)/10;
	motor_vel[1] = (int_sin((curr_angle+1200)%3600)*(s32)curr_speed*(-1)/10000 + curr_w)/10;
	motor_vel[2] = (int_sin((curr_angle+2400)%3600)*(s32)curr_speed*(-1)/10000 + curr_w)/10;
	
	for (u8 i=0;i<3;i++){
		motor_loop_state[i] = CLOSE_LOOP;
	}
	
	s16 motor_vel_max = motor_vel[0];
	for (u8 i=1; i<3; i++){
		if (abs(motor_vel[i])>abs(motor_vel_max)){
			motor_vel_max = motor_vel[i];
		}
	}
	
	if (abs(motor_vel_max)>MOTOR_MAX_MY_BOUND){
		s32 motor_ratio = MOTOR_MAX_MY_BOUND*10000/abs(motor_vel_max); //Scaled by 10000
		for (u8 i=0;i<3;i++){
			motor_vel[i] = (s32)motor_vel[i]*motor_ratio/10000;
		}
	}
	
	for (u8 i=0;i<3;i++){
		motor_set_vel((MOTOR_ID)(MOTOR1 + i), motor_vel[i], motor_loop_state[i]);
	}
}

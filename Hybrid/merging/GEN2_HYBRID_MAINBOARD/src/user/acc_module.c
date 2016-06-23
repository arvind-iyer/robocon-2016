#include "manual_track.h"

s16 curr_vx = 0, curr_vy = 0, curr_w = 0;
s16 motor_vel[3] = {0};
CLOSE_LOOP_FLAG motor_loop_state[3] = {CLOSE_LOOP};
static s16 accel_remain[3] = {0}; //vx, vy, w
static s32 last_ticks = 0;

//Factor scaled by 1000
static s16 get_new_v(s16 curr_v, s16 target_v, s16 accel_factor, s16 decel_factor, s16 *remainder){
	s32 accel_amount = *remainder;
	s16 v_diff = target_v - curr_v;
	if (v_diff > 0){
		accel_amount += accel_factor*v_diff;
	}else{
		accel_amount += decel_factor*v_diff;
	}
	
	if (abs(v_diff) <= abs(accel_amount/1000)){
		*remainder = 0;
		return target_v;
	}else{
		*remainder = accel_amount%1000;
		return curr_v + accel_amount/1000;
	}
}

//Update in local coordinate
//All acceleration scaled by 1000
void acc_update(s16 vx, s16 vy, s16 w, s16 v_acc, s16 v_dec, s16 w_acc, s16 w_dec, bool global){
	
	u16 ticks_diff = this_loop_ticks - last_ticks;
	ticks_diff = 3;
	
//	if (ticks_diff < SHORT_LOOP_TICKS*10){
//		v_acc *= ticks_diff;
//		v_dec *= ticks_diff;
//		w_acc *= ticks_diff;
//		w_dec *= ticks_diff;
//	}
	
	s16 rotate_accel_amount = 0;
	if (abs(w) > abs(curr_w)){
		rotate_accel_amount = w_acc + rotate_accel_remainder;
	}else{
		rotate_accel_amount = w_dec + rotate_accel_remainder;
	}
	
	rotate_accel_remainder = rotate_accel_amount % 1000;
	rotate_accel_amount /= 1000;
	if (abs(w-curr_w) < abs(rotate_accel_amount+1)){
		curr_w = w;
	}else{
		if (w > curr_w){
			curr_w += rotate_accel_amount;
		}else{
			curr_w -= rotate_accel_amount;
		}
	}
	
	s16 acceleration_amount;
	if ((abs(vx + vy)) > (abs(curr_vx + curr_vy))){
		acceleration_amount = v_acc + accel_remainder; //Scaled by 1000
	}else{
		acceleration_amount = v_dec + accel_remainder; //Scaled by 1000
	}
	
	accel_remainder = acceleration_amount % 1000;
	acceleration_amount /= 1000;
	
	//If the difference is not that much, directly assign speed
	if (abs(curr_vx - vx) < (acceleration_amount+1)){
		curr_vx = vx;
	}else{
		if (curr_vx > vx){
			curr_vx -= acceleration_amount;
		}else{
			curr_vx += acceleration_amount;
		}
	}
	
	if (abs(curr_vy - vy) < (acceleration_amount+1)){
		curr_vy = vy;
	}else{
		if (curr_vy > vy){
			curr_vy -= acceleration_amount;
		}else{
			curr_vy += acceleration_amount;
		}
	}
	
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
	
	last_ticks = this_loop_ticks;
}

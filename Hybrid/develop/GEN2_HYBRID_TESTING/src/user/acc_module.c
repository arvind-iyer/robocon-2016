#include "manual_track.h"

s32 curr_vx = 0, curr_vy = 0, curr_w = 0;
s32 motor_vel[3] = {0};
CLOSE_LOOP_FLAG motor_loop_state[3] = {CLOSE_LOOP};
static s32 accel_remainder = 0;
static s32 rotate_accel_remainder = 0;

//Update in local coordinate
//All acceleration scaled by 1000
void acc_update(s32 vx, s32 vy, s32 w, s32 v_acc, s32 v_dec, s32 w_acc, s32 w_dec){
	
	s32 rotate_accel_amount = 0;
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
	
	u16 acceleration_amount;
	if ((vx*vx+vy*vy) > (curr_vx*curr_vx + curr_vy*curr_vy)){
		acceleration_amount = v_acc + accel_remainder; //Scaled by 1000
	}else{
		acceleration_amount = v_dec + accel_remainder; //Scaled by 1000
	}
	
	accel_remainder = acceleration_amount % 1000;
	acceleration_amount /= 1000;
	
	//If the difference is not that much, directly assign speed
	if (Abs(curr_vx - vx) < (acceleration_amount+1) && Abs(curr_vy - vy) < (acceleration_amount+1)){
		curr_vx = vx;
		curr_vy = vy;
	}else{
		if (curr_vx > vx){
			curr_vx -= acceleration_amount;
		}else{
			curr_vx += acceleration_amount;
		}
		if (curr_vy > vy){
			curr_vy -= acceleration_amount;
		}else{
			curr_vy += acceleration_amount;
		}
	}
	
	s32 curr_angle = int_arc_tan2(-curr_vy, curr_vx)*10;
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

#include "manual_move.h"

//Scaled by 10
s32 target_vx = 0;
s32 target_vy = 0;
s32 target_rotate = 0;
s32 curr_vx = 0;
s32 curr_vy = 0;
s32 curr_rotate = 0;
s32 curr_angle = 0;
u16 target_ticks = 0;
u32 orig_ticks = 0;

LOCK_STATE is_climbing = LOCKED;
LOCK_STATE brushless_unlock_state = LOCKED;
LOCK_STATE emergency_lock = UNLOCKED;
u16 brushless_time_count = 0;
LOCK_STATE pneumatic_state = UNLOCKED;
LOCK_STATE press_button_B = UNLOCKED;
LOCK_STATE press_button_back = UNLOCKED;
s8 cache_speed_up = 0;

void manual_init(){
	xbc_mb_init(XBC_CAN_FIRST);
}

void brushless_control(s16 value){
	if (value>BRUSHLESS_MAX){
		servo_control_all(BRUSHLESS_MAX);
	}else if(value<BRUSHLESS_MIN){
		servo_control_all(BRUSHLESS_MIN);
	}else{
		servo_control_all(value);
	}
}

void climb_continue(){
	if (emergency_lock==UNLOCKED){
		motor_set_vel(MOTOR4, CLIMBING_SPEED, OPEN_LOOP);
		motor_set_vel(MOTOR5, CLIMBING_SPEED, OPEN_LOOP);
		motor_set_vel(MOTOR6, CLIMBING_SPEED, OPEN_LOOP);
	}
}

void stop_climbing(){
	motor_set_vel(MOTOR4, 0, OPEN_LOOP);
	motor_set_vel(MOTOR5, 0, OPEN_LOOP);
	motor_set_vel(MOTOR6, 0, OPEN_LOOP);
}

CONTROL_STATE manual_control_update(){
	if (button_pressed(BUTTON_XBC_XBOX)){
		curr_vx = curr_vy = curr_rotate = target_vx = target_vy = target_rotate = brushless_time_count = 0;
		brushless_unlock_state = is_climbing = LOCKED;
		emergency_lock = UNLOCKED;
		brushless_control(BRUSHLESS_MIN);
		
		motor_set_vel(MOTOR1, 0, CLOSE_LOOP);
		motor_set_vel(MOTOR2, 0, CLOSE_LOOP);
		motor_set_vel(MOTOR3, 0, CLOSE_LOOP);
		motor_set_vel(MOTOR4, 0, CLOSE_LOOP);
		motor_set_vel(MOTOR5, 0, CLOSE_LOOP);
		motor_set_vel(MOTOR6, 0, CLOSE_LOOP);
		return AUTO_MODE;
	}
	
	//Calcuate 3 base wheels movement
	s32 vx = xbc_get_joy(XBC_JOY_LX);
	s32 vy = xbc_get_joy(XBC_JOY_LY);
	curr_angle = int_arc_tan2(curr_vx, curr_vy)*10 + 1800;
//	if ((vx-curr_vx)*(vx-curr_vx)>ACC_THRESHOLD || (vy-curr_vy)*(vy-curr_vy)>ACC_THRESHOLD){
//		curr_vx = (curr_vx*99/100 + vx*1/100);
//		curr_vy = (curr_vy*99/100 + vy*1/100);
//	}else{
//		curr_vx = vx;
//		curr_vy = vy;
//	}
	curr_vx = (curr_vx*99/100 + vx*1/100);
	curr_vy = (curr_vy*99/100 + vy*1/100);
	curr_rotate = xbc_get_joy(XBC_JOY_LT)*10/6 - xbc_get_joy(XBC_JOY_RT)*10/6;

	if (emergency_lock==UNLOCKED){
		s32 curr_speed = (curr_vx*curr_vx + curr_vy*curr_vy) / 800;
		s32 vel1 = (int_sin(curr_angle)*curr_speed*(-1)/10000 + curr_rotate)/10;
		s32 vel2 = (int_sin(curr_angle+1200)*curr_speed*(-1)/10000 + curr_rotate)/10;
		s32 vel3 = (int_sin(curr_angle+2400)*curr_speed*(-1)/10000 + curr_rotate)/10;
		
		motor_set_vel(MOTOR1, vel1, (vel1*vel1)<9?OPEN_LOOP:CLOSE_LOOP);
		motor_set_vel(MOTOR2, vel2, (vel2*vel2)<9?OPEN_LOOP:CLOSE_LOOP);
		motor_set_vel(MOTOR3, vel3, (vel3*vel3)<9?OPEN_LOOP:CLOSE_LOOP);
		tft_clear();
		tft_append_line("%d", curr_vx);
		tft_append_line("%d", curr_vy);
		tft_append_line("%d", curr_speed);
		tft_append_line("%d", curr_rotate);
		tft_append_line("%d %d %d", vel1, vel2, vel3);
		tft_append_line("%d", get_ticks());
		tft_update();
	}
		
	//Calcuate brushless
	//If locked, long down unlock
	if (brushless_unlock_state==LOCKED){
		brushless_control(0);
		if (xbc_get_joy(XBC_JOY_RY)<-900 && button_pressed(BUTTON_XBC_R_JOY)){
			if (brushless_time_count == 0){
				brushless_time_count = get_seconds();
			}else if(brushless_time_count>1){
				brushless_unlock_state = UNLOCKED;
				brushless_time_count = 0;
			}
		}
	
	//If unlocked, long down to lock again
	}else{
		if (xbc_get_joy(XBC_JOY_RY)<-900 && button_pressed(BUTTON_XBC_R_JOY)){
			if (brushless_time_count == 0){
				brushless_time_count = get_seconds();
			}else if(brushless_time_count>1){
				brushless_unlock_state = LOCKED;
				brushless_time_count = 0;
			}
		}
		brushless_control(xbc_get_joy(XBC_JOY_RY)*1050/(BRUSHLESS_MAX-BRUSHLESS_MIN)+BRUSHLESS_MIN);
	}
	
	//Calcuate climbing wheels
	if (button_pressed(BUTTON_XBC_A)){
		climb_continue();
	}else{
		stop_climbing();
	}
	
	//Pneumatic control
	if (button_pressed(BUTTON_XBC_B)){
		if (press_button_B == UNLOCKED){
			press_button_B = LOCKED;
			pneumatic_control(GPIOB, GPIO_Pin_9, pneumatic_state);
			pneumatic_state = !pneumatic_state;
		}
	}else{
		press_button_B = UNLOCKED;
	}
	
	//E-Stop
	if (emergency_lock==UNLOCKED && button_pressed(BUTTON_XBC_BACK)){
		if (press_button_back == UNLOCKED){
			press_button_back = LOCKED;
			motor_lock(MOTOR1);
			motor_lock(MOTOR2);
			motor_lock(MOTOR3);
			motor_lock(MOTOR4);
			motor_lock(MOTOR5);
			motor_lock(MOTOR6);
			brushless_unlock_state = LOCKED;
			brushless_control(0);
			emergency_lock = LOCKED;
		}else{
			press_button_back = UNLOCKED;
		}
	}
	//Un E-Stop
//	if (emergency_lock==LOCKED && button_pressed(BUTTON_XBC_BACK)){
//		if (press_button_back == UNLOCKED){
//			press_button_back = LOCKED;
//			brushless_unlock_state = UNLOCKED;
//			emergency_lock = UNLOCKED;
//		}else{
//			press_button_back = UNLOCKED;
//		}
//	}
	return MANUAL_CONTROL;
}

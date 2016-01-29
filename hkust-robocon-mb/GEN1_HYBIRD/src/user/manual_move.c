#include "manual_move.h"

s32 target_speed = 0;
s32 target_rotate = 0;
s32 curr_speed = 0;
s32 curr_angle = 0;
s32 curr_rotate = 0;
u8 is_climbing = LOCKED;
u8 brushless_unlock_state = LOCKED;
u8 emergency_lock = UNLOCKED;
u16 brushless_time_count = 0;
u8 pneumatic_state = 0;

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
	motor_set_vel(MOTOR4, CLIMBING_SPEED, OPEN_LOOP);
	motor_set_vel(MOTOR5, CLIMBING_SPEED, OPEN_LOOP);
	motor_set_vel(MOTOR6, CLIMBING_SPEED, OPEN_LOOP);
}

void stop_climbing(){
	motor_set_vel(MOTOR4, 0, OPEN_LOOP);
	motor_set_vel(MOTOR5, 0, OPEN_LOOP);
	motor_set_vel(MOTOR6, 0, OPEN_LOOP);
}

CONTROL_STATE manual_control_update(){
	if (button_pressed(BUTTON_XBC_XBOX)){
		curr_speed = curr_angle = curr_rotate = target_speed = target_rotate = brushless_time_count = 0;
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
	s32 angle = int_arc_tan2(xbc_get_joy(XBC_JOY_LX), xbc_get_joy(XBC_JOY_LY))*10;
	s32 speed = (xbc_get_joy(XBC_JOY_LY)*xbc_get_joy(XBC_JOY_LY) + 
								xbc_get_joy(XBC_JOY_LX)*xbc_get_joy(XBC_JOY_LX))/10000*60/100;
	s8 rotate = xbc_get_joy(XBC_JOY_LT)/6 - xbc_get_joy(XBC_JOY_RT)/6;
	
	set_global_movement(angle, speed, rotate);
	
	if ((curr_speed*curr_speed - target_speed*target_speed)>ACC_THRESHOLD){
		if (curr_speed>target_speed){
			curr_speed -= ACC_STEPPING;
		}else{
			curr_speed += ACC_STEPPING;
		}
	}else{
		curr_speed = target_speed;
	}
	
	if ((curr_rotate*curr_rotate - target_rotate*target_rotate)>ACC_THRESHOLD){
		if (curr_rotate>target_rotate){
			curr_rotate -= ACC_STEPPING;
		}else{
			curr_rotate += ACC_STEPPING;
		}
	}else{
		curr_rotate = target_rotate;
	}
	
	if (emergency_lock==UNLOCKED){
		motor_set_vel(MOTOR1, int_sin(curr_angle)*curr_speed*(-1)/10000 + curr_rotate, CLOSE_LOOP);
		motor_set_vel(MOTOR2, int_sin(curr_angle+1200)*curr_speed*(-1)/10000 + curr_rotate, CLOSE_LOOP);
		motor_set_vel(MOTOR3, int_sin(curr_angle+2400)*curr_speed*(-1)/10000 + curr_rotate, CLOSE_LOOP);
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
		pneumatic_control(GPIOB, GPIO_Pin_9, pneumatic_state);
		pneumatic_state = !pneumatic_state;
	}
	
	//E-Stop
	if (emergency_lock==UNLOCKED && button_pressed(BUTTON_XBC_BACK)){
		motor_lock(MOTOR1);
		motor_lock(MOTOR2);
		motor_lock(MOTOR3);
		motor_lock(MOTOR4);
		motor_lock(MOTOR5);
		motor_lock(MOTOR6);
		brushless_unlock_state = LOCKED;
		brushless_control(0);
		emergency_lock = LOCKED;
	}
	
	//Recover from E-Stop by hitting LB/RB/Start/Back together
	if (emergency_lock==LOCKED && button_pressed(BUTTON_XBC_BACK) && button_pressed(BUTTON_XBC_LB)
		&& button_pressed(BUTTON_XBC_RB) && button_pressed(BUTTON_XBC_START)){
		brushless_unlock_state = UNLOCKED;
		emergency_lock = UNLOCKED;
	}
	return MANUAL_CONTROL;
}

//Angle Scaled by 10
void set_global_movement(s32 angle, s32 speed, s32 rotate){
	curr_angle = angle;
	target_speed = speed;
	target_rotate = rotate;
}
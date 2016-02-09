/**
** This file is to handle the manual control part of the robot
**
** Control scheme:
** Left joy - Base Movement
** Right joy - Self rotation
** LB & RB - Unlock the brushless, will re-lock after 5 seconds if not used
** LT & RT - Control the speed of brushless motors respectively
** Button B - Set/Reset the pneumatic tube
** Button A & Y - Control the climbing/desending of the robot
** Button X - Lock the robot in place using pid
**/

#include "manual_mode.h"

s32 curr_vx = 0, curr_vy = 0, curr_rotate = 0;
LOCK_STATE is_climbing = LOCKED;
LOCK_STATE brushless_lock = UNLOCKED;
LOCK_STATE ground_wheels_lock = UNLOCKED;
LOCK_STATE climbing_induced_ground_lock = UNLOCKED;
LOCK_STATE press_button_B = UNLOCKED;
LOCK_STATE press_button_X = UNLOCKED;

u16 brushless_pressed_time[2] = {0};

static XBC_JOY brushless_joy_sticks[2] = {XBC_JOY_LT, XBC_JOY_RT};
static u16 brushless_stick_max = 255;

void manual_init(){
	xbc_mb_init(XBC_CAN_FIRST);
	manual_reset();
}

void manual_reset(){
	curr_vx = curr_vy = curr_rotate = 0;
	brushless_lock = UNLOCKED;
	ground_wheels_lock = UNLOCKED;
	climbing_induced_ground_lock = UNLOCKED;
	press_button_B = press_button_X = UNLOCKED;
	brushless_control_all(0, true);
}

//This fast update is for controlling things that require a high refresh rate
void manual_fast_update(){
	if (ground_wheels_lock == LOCKED){
		//_lockInTarget();
	}
}

//Interval update is called in a timed interval, reducing stress
void manual_interval_update(){
	tft_clear();
	
	if (ground_wheels_lock == UNLOCKED){
		if (climbing_induced_ground_lock == UNLOCKED){
			//Calcuate 3 base wheels movement
			s32 vx = xbc_get_joy(XBC_JOY_LX);
			s32 vy = xbc_get_joy(XBC_JOY_LY);
			
			//Plus 1800 to switch heading
			//curr_angle = int_arc_tan2(curr_vx, curr_vy)*10 + 1800;
			
			//Put acceleration
			if (Abs(curr_vx - vx) > ACC_THRESHOLD || Abs(curr_vy - vy) > ACC_THRESHOLD){
				curr_vx = (curr_vx*(1000-ACC_RATE)/1000 + vx*ACC_RATE/1000);
				curr_vy = (curr_vy*(1000-ACC_RATE)/1000 + vy*ACC_RATE/1000);
			}else{
				curr_vx = vx;
				curr_vy = vy;
			}
			
			s32 curr_angle = int_arc_tan2(curr_vx, curr_vy)*10;
			curr_rotate = -xbc_get_joy(XBC_JOY_RX)/3;

			s32 curr_speed = (curr_vx*curr_vx + curr_vy*curr_vy) / 300;
			s32 motor_vel[3] = {0};
			motor_vel[0] = (int_sin(curr_angle)*curr_speed*(-1)/10000 + curr_rotate)/10;
			motor_vel[1] = (int_sin(curr_angle+1200)*curr_speed*(-1)/10000 + curr_rotate)/10;
			motor_vel[2] = (int_sin(curr_angle+2400)*curr_speed*(-1)/10000 + curr_rotate)/10;
			
			for (u8 i=0;i<3;i++){
				//motor_set_vel(MOTOR1 + i, motor_vel[i], motor_vel[i]==0?OPEN_LOOP:CLOSE_LOOP);
				motor_set_vel(MOTOR1 + i, motor_vel[i], CLOSE_LOOP);
			}
			tft_append_line("%d %d %d", motor_vel[0], motor_vel[1], motor_vel[2]);
			tft_append_line("%d %d %d", get_target_vel(MOTOR1), get_target_vel(MOTOR2), get_target_vel(MOTOR3));
			tft_append_line("%d %d %d", get_curr_vel(MOTOR1), get_curr_vel(MOTOR2), get_curr_vel(MOTOR3));
			tft_append_line("%d %d %d", get_pwm_value(MOTOR1)/10000, get_pwm_value(MOTOR2)/10000, get_pwm_value(MOTOR3)/10000);
		}else{
			for (u8 i=0;i<3;i++){
				//motor_set_vel(MOTOR1 + i, motor_vel[i], motor_vel[i]==0?OPEN_LOOP:CLOSE_LOOP);
				motor_set_vel(MOTOR1 + i, 0, OPEN_LOOP);
			}
		}
	}
	
	tft_append_line("%d", this_loop_ticks);
	
	//Calcuate brushless
	if (brushless_lock == UNLOCKED){
		if (button_pressed(BUTTON_XBC_LB) && button_pressed(BUTTON_XBC_RB)){
			//Run the brushless
			//tft_append_line("B:%d %d", xbc_get_joy(XBC_JOY_LT)*(BRUSHLESS_MAX-BRUSHLESS_MIN)/255+BRUSHLESS_MIN, xbc_get_joy(XBC_JOY_RT)*(BRUSHLESS_MAX-BRUSHLESS_MIN)/255+BRUSHLESS_MIN);
			//brushless_control(BRUSHLESS_1, xbc_get_joy(XBC_JOY_LT)*(BRUSHLESS_MAX-BRUSHLESS_MIN)/255+BRUSHLESS_MIN);
			//brushless_control(BRUSHLESS_2, xbc_get_joy(XBC_JOY_RT)*(BRUSHLESS_MAX-BRUSHLESS_MIN)/255+BRUSHLESS_MIN);

			/* Control Manual: 
			** first 30% -> No respond
			** 30% ~ 50% -> Able to make eco start moving (constant across)
			** 50% ~ 100% -> Constant 50%
			** Keep 100% -> Continue grow to max power
			*/
			for (u8 i=0;i<BRUSHLESS_COUNT;i++){
				if (xbc_get_joy(brushless_joy_sticks[i]) < (brushless_stick_max*3/10)){
					brushless_pressed_time[i] = 0;
					brushless_control((BRUSHLESS_ID)i, 0, true);
					tft_append_line("%d%", 0);
					
				}else if(xbc_get_joy(brushless_joy_sticks[i]) < (brushless_stick_max/2)){
					brushless_pressed_time[i] = 0;
					brushless_control((BRUSHLESS_ID)i, 35, true);
					tft_append_line("%d%", 35);
					
				}else{
					//Cap the brushless_pressed_time to avoid overflow
					u16 ticks_different = (this_loop_ticks - last_long_loop_ticks);
					if ((brushless_pressed_time[i] + ticks_different) < 10000){
						brushless_pressed_time[i] += ticks_different;
					}else{
						brushless_pressed_time[i] = 10000;
					}
					
					//If less than 0.7 seconds, keep 50%
					if (brushless_pressed_time[i]<700){
						u16 Iamjustatempvariable = 35 + (xbc_get_joy(brushless_joy_sticks[i] - brushless_stick_max/2))*15/128;
						brushless_control((BRUSHLESS_ID)i, 50, true);
						tft_append_line("%d%", Iamjustatempvariable);
					}else{
						u16 Iamjustatempvariable = 50 + (brushless_pressed_time[i]-700)/20;
						Iamjustatempvariable = Iamjustatempvariable>100?100:Iamjustatempvariable;
						brushless_control((BRUSHLESS_ID)i, Iamjustatempvariable, true);
						tft_append_line("%d%", Iamjustatempvariable);
					}
				}
			}
		}else{
			brushless_control_all(0, true);
			for (u8 i=0;i<BRUSHLESS_COUNT;i++){
				brushless_pressed_time[i] = 0;
			}
		}
	}
	
	//Deal with climbing
	if (button_pressed(BUTTON_XBC_A) > XBC_BUTTON_MIN_PRESSED){
		climb_continue();
		climbing_induced_ground_lock = LOCKED;
		tft_append_line("CLIMBING");
	}else	if (button_pressed(BUTTON_XBC_Y) > XBC_BUTTON_MIN_PRESSED){
		descend_continue();
		climbing_induced_ground_lock = LOCKED;
		tft_append_line("DESCENDING");
	}else{
		stop_climbing();
		climbing_induced_ground_lock = UNLOCKED;
	}
	
	//Locking the motors
	//TODO: Add PID locking
	if (button_pressed(BUTTON_XBC_X) > XBC_BUTTON_MIN_PRESSED){
		if (press_button_X == UNLOCKED){
			press_button_X = LOCKED;
			buzzer_beep(75);
			if (ground_wheels_lock == UNLOCKED){
				ground_wheels_lock = LOCKED;
				_setCurrentAsTarget();
			}else {
				buzzer_beep(225);
				ground_wheels_lock = UNLOCKED;
			}
		}
	}else{
			press_button_X = UNLOCKED;
	}
	
	//Pneumatic control
	if (button_pressed(BUTTON_XBC_B) > XBC_BUTTON_MIN_PRESSED){
		if (press_button_B == UNLOCKED){
			press_button_B = LOCKED;
			pneumatic_toggle();
		}
	}else{
		press_button_B = UNLOCKED;
	}
	tft_update();
}

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

static s32 curr_vx = 0, curr_vy = 0, curr_angle = 0;
static s32 curr_heading = 0;
static s32 actual_speed[2] = {0};
static s32 encoder_last_vel[2][ENCODER_READING_ARRAY_SIZE] = {0};
static u16	encoder_reading_pointer[2] = {0};

static s32 motor_vel[3] = {0};
static CLOSE_LOOP_FLAG motor_loop_state[3] = {CLOSE_LOOP};
static bool is_rotating = false;

static LOCK_STATE brushless_lock = UNLOCKED;
static LOCK_STATE ground_wheels_lock = UNLOCKED;
static LOCK_STATE climbing_induced_ground_lock = UNLOCKED;
static LOCK_STATE press_button_B = UNLOCKED;
static LOCK_STATE press_button_X = UNLOCKED;

static u16 brushless_pressed_time[2] = {0};
static u16 brushless_lock_timeout = BRUSHLESS_LOCK_TIMEOUT+1;
static XBC_JOY brushless_joy_sticks[2] = {XBC_JOY_LT, XBC_JOY_RT};
static u16 brushless_stick_max = 255;

static s16 last_angle_pid = 0;
static s32 sum_of_last_angle_error = 0;

void manual_init(){
	xbc_mb_init(XBC_CAN_FIRST);
	manual_reset();
}

void manual_reset(){
	curr_vx = curr_vy = curr_angle = curr_heading = 0;
	brushless_lock = UNLOCKED;
	ground_wheels_lock = UNLOCKED;
	climbing_induced_ground_lock = UNLOCKED;
	press_button_B = press_button_X = UNLOCKED;
	is_rotating = false;
	brushless_lock_timeout = BRUSHLESS_LOCK_TIMEOUT + 1;
	brushless_control_all(0, true);
}

void manual_vel_set_zero(){
	for (u8 i=0;i<3;i++){
		motor_vel[i] = 0;
		motor_loop_state[i] = OPEN_LOOP;
	}
}

//Do PID in angle
s32 angle_pid(){
	s16 this_curr_global_angle = get_angle();
	s16 this_target_global_angle = curr_heading;
	//Map both angles to same area
	while(this_curr_global_angle>=1800){
		this_curr_global_angle -= 3600;
	}
	while(this_curr_global_angle<-1800){
		this_curr_global_angle += 3600;
	}
	while(this_target_global_angle>=1800){
		this_target_global_angle -= 3600;
	}
	while(this_target_global_angle<-1800){
		this_target_global_angle += 3600;
	}
	s32 error = this_target_global_angle - this_curr_global_angle;
	sum_of_last_angle_error = sum_of_last_angle_error*98/99 + error;
	//This value scaled by 1000(PID constant) and then 10(angle scale)
	s32 temp = error*ANGLE_PID_P + sum_of_last_angle_error*ANGLE_PID_I + (this_curr_global_angle-last_angle_pid)*ANGLE_PID_D;
	last_angle_pid = this_curr_global_angle;
	
	if (temp > ANGLE_PID_MAX){
		temp = ANGLE_PID_MAX;
	}else if (temp < -ANGLE_PID_MAX){
		temp = -ANGLE_PID_MAX;
	}
	return temp;
}

/*
** This fast update is for controlling things that require a high refresh rate
** It contains:
** - Locking itself in place
** - Recording encoder reading and speed
*/
void manual_fast_update(){
	if (ground_wheels_lock == LOCKED){
		//_lockInTarget();
		s32 curr_rotate = -angle_pid()/1500;
		for (u8 i=0;i<3;i++){
			motor_vel[i] = curr_rotate/10;
			motor_loop_state[i] = CLOSE_LOOP;
			motor_set_vel((MOTOR_ID)MOTOR1 + i, motor_vel[i], motor_loop_state[i]);
		}
	}
	
	//Recording encoder velocity, need to connect encoder to mainboard
	for (u8 counter=0;counter<2;counter++){
		//s32 encoder_reading = get_encoder_count((ENCODER)counter);
		//encoder_last_vel[counter][encoder_reading_pointer[counter]] = encoder_reading;
		//encoder_reading_pointer[counter] = (encoder_reading_pointer[counter]+1) % ENCODER_READING_ARRAY_SIZE;
		//actual_speed[counter] = encoder_reading - (encoder_reading_pointer[counter]+1) % ENCODER_READING_ARRAY_SIZE;
	}
}

//Interval update is called in a timed interval, reducing stress
void manual_interval_update(){
	tft_clear();
	u16 ticks_different = (this_loop_ticks - last_long_loop_ticks);
	
	if (ground_wheels_lock == UNLOCKED){
		if (climbing_induced_ground_lock == UNLOCKED){
			//Calcuate 3 base wheels movement
			s32 vx = xbc_get_joy(XBC_JOY_LX);
			s32 vy = xbc_get_joy(XBC_JOY_LY);
			
			/*
			** This part calculate acceleration
			** It caps the max. acceleration for both x and y
			** By finding the axis with more difference(thus more acceleration), cap it
			** Then use the same proportion for the another axis to ensure the angle is correct
			*/
			u8 acceleration_amount;
			//If both x and y are at low speed, use low speed mode
			if (Abs(curr_vx) < LOW_SPEED_THRESHOLD && Abs(curr_vy) < LOW_SPEED_THRESHOLD){
				acceleration_amount = LOW_SPEED_ACC;
			}else {
				acceleration_amount = HIGH_SPEED_ACC;
			}
			
			//If the difference is not that much, directly assign speed
			if (Abs(curr_vx - vx) < (acceleration_amount+1) && Abs(curr_vy - vy) < (acceleration_amount+1)){
				curr_vx = vx;
				curr_vy = vy;
			
			//Else:
			//Use the axis with larger difference as the major consideration
			//The other axis simply follow the proportion
			}else if (Abs(curr_vx - vx) > Abs(curr_vy - vy)){
				//Use x-axis as major
				s32 proportion;
				if (curr_vx > vx){
					proportion = acceleration_amount *1000 / ((s32)curr_vx - vx);
					curr_vx -= acceleration_amount;
				}else{
					proportion = acceleration_amount *1000 / ((s32)vx - curr_vx);
					curr_vx += acceleration_amount;
				}
				
				curr_vy += (vy - curr_vy) *proportion /1000;
				
			}else{
				//Use y-axis as major
				s32 proportion;
				if (curr_vy > vy){
					proportion = acceleration_amount *1000 / ((s32)curr_vy - vy);
					curr_vy -= acceleration_amount;
				}else{
					proportion = acceleration_amount *1000 / ((s32)vy - curr_vy);
					curr_vy += acceleration_amount;
				}
				
				curr_vx += (vx - curr_vx) *proportion /1000;
			}
			
			curr_angle = int_arc_tan2(curr_vx, curr_vy)*10;
			s32 curr_rotate = -xbc_get_joy(XBC_JOY_RX)/3;
			//change heading for angle PID use
			if (curr_rotate == 0){
				if (is_rotating){
					is_rotating = false;
					curr_heading = get_angle();
				}
				curr_rotate = -angle_pid()/1500;
			}else{
				is_rotating = true;
			}

			s32 curr_speed = (curr_vx*curr_vx + curr_vy*curr_vy) / 700;
			motor_vel[0] = (int_sin(curr_angle)*curr_speed*(-1)/10000 + curr_rotate)/10;
			motor_vel[1] = (int_sin(curr_angle+1200)*curr_speed*(-1)/10000 + curr_rotate)/10;
			motor_vel[2] = (int_sin(curr_angle+2400)*curr_speed*(-1)/10000 + curr_rotate)/10;
			
			for (u8 i=0;i<3;i++){
				motor_loop_state[i] = CLOSE_LOOP;
			}
			tft_append_line("%d %d", curr_vx, curr_vy);
			tft_append_line("%d", curr_rotate);
			//tft_append_line("%d %d %d", get_target_vel(MOTOR1), get_target_vel(MOTOR2), get_target_vel(MOTOR3));
			//tft_append_line("%d %d %d", get_curr_vel(MOTOR1), get_curr_vel(MOTOR2), get_curr_vel(MOTOR3));
			//tft_append_line("%d %d %d", get_pwm_value(MOTOR1)/10000, get_pwm_value(MOTOR2)/10000, get_pwm_value(MOTOR3)/10000);
		}else{
			for (u8 i=0;i<3;i++){
				motor_vel[i] = 0;
				motor_loop_state[i] = OPEN_LOOP;
			}
		}
	}
	
	tft_append_line("%d", this_loop_ticks);
	tft_append_line("%d", this_loop_ticks-last_long_loop_ticks);
	
	/* 
	** This part provide the safety lock and control for the brushless motors
	**
	** Control Manual: 
	** first 20% -> No respond
	** 20% ~ 33% -> Able to make eco start moving (constant across)
	** 33% ~ 100% -> grow linearly to 80%
	** Keep 100% -> Continue grow to max power wih respect to time
	*/
	if (brushless_lock == UNLOCKED){
		//Run the brushless

		//If not quite pressed, count time
		if (xbc_get_joy(brushless_joy_sticks[0]) < (brushless_stick_max/10) && xbc_get_joy(brushless_joy_sticks[1]) < (brushless_stick_max/10)){
				if ((brushless_lock_timeout + ticks_different) < BRUSHLESS_LOCK_TIMEOUT){
					brushless_lock_timeout += ticks_different;
				}else{
					brushless_lock = LOCKED;
					brushless_lock_timeout = BRUSHLESS_LOCK_TIMEOUT + 1;
				}
		}else{
			brushless_lock_timeout = 0;
		}
		
		for (u8 i=0;i<BRUSHLESS_COUNT;i++){

			u16 pressed_power = xbc_get_joy(brushless_joy_sticks[i]);
			
			if (pressed_power < (brushless_stick_max*2/10)){
				brushless_pressed_time[i] = 0;
				brushless_control((BRUSHLESS_ID)i, 0, true);
				tft_append_line("B%d Z: %d%", i, 0);
				
			}else if(pressed_power < (brushless_stick_max*3/10)){
				brushless_pressed_time[i] = 0;
				brushless_control((BRUSHLESS_ID)i, 40, true);
				tft_append_line("B%d C:%d%", i, 40);
				
			}else if(pressed_power <= (brushless_stick_max*9/10)){
				brushless_pressed_time[i] = 0;
				brushless_control((BRUSHLESS_ID)i, 40 + (pressed_power-brushless_stick_max*3/10)*40/(brushless_stick_max*(99-30)/100), true);
				tft_append_line("B%d L:%dK", i, 40 + (pressed_power-brushless_stick_max*3/10)*40/(brushless_stick_max*(99-30)/100));
				
			}else{
				//Cap the brushless_pressed_time to avoid overflow
				if ((brushless_pressed_time[i] + ticks_different) < 10000){
					brushless_pressed_time[i] += ticks_different;
				}else{
					brushless_pressed_time[i] = 10000;
				}
				
				if (brushless_pressed_time[i]<1000){
					brushless_control((BRUSHLESS_ID)i, 80, true);
					tft_append_line("B%d C:%d%", i, 80);
				}else{
					u16 Iamjustatempvariable = 80 + (brushless_pressed_time[i]-1000)/100;
					Iamjustatempvariable = Iamjustatempvariable>100?100:Iamjustatempvariable;
					brushless_control((BRUSHLESS_ID)i, Iamjustatempvariable, true);
					tft_append_line("B%d G:%d%", i, Iamjustatempvariable);
				}
			}
		}
	}else if (button_pressed(BUTTON_XBC_LB) && button_pressed(BUTTON_XBC_RB)){
		brushless_lock = UNLOCKED;
		brushless_lock_timeout = 0;
	}else{
		brushless_control_all(0, true);
		for (u8 i=0;i<BRUSHLESS_COUNT;i++){
			brushless_pressed_time[i] = 0;
		}
	}
	
	/*
	** This part is related to climbing mechanism
	** Hold button A to climb up
	** Hold button Y to descend
	** While climbing, the ground wheels should be locked
	*/
	if (button_pressed(BUTTON_XBC_A)){
		climb_continue();
		climbing_induced_ground_lock = LOCKED;
		tft_append_line("CLIMBING");
	}else	if (button_pressed(BUTTON_XBC_Y)){
		descend_continue();
		climbing_induced_ground_lock = LOCKED;
		tft_append_line("DESCENDING");
	}else{
		stop_climbing();
		climbing_induced_ground_lock = UNLOCKED;
	}

	/*
	** This part deals with locking the robot.
	** When the lock button(X) is pressed, the ground wheels should be locked at least
	** For further implementation, it should lock itself in place with PID
	*/
	//TODO: Add PID locking
	if (button_pressed(BUTTON_XBC_X)){
		if (press_button_X == UNLOCKED){
			press_button_X = LOCKED;
			buzzer_beep(75);
			if (ground_wheels_lock == UNLOCKED){
				curr_heading = get_angle();
				ground_wheels_lock = LOCKED;
				_setCurrentAsTarget();
				manual_vel_set_zero();
			}else {
				buzzer_beep(225);
				ground_wheels_lock = UNLOCKED;
				manual_vel_set_zero();
			}
		}
	}else{
			press_button_X = UNLOCKED;
	}
	
	/*
	** This part deals with locking the robot onto the pole.
	*/
	if (button_pressed(BUTTON_XBC_B)){
		if (press_button_B == UNLOCKED){
			press_button_B = LOCKED;
			pneumatic_toggle();
		}
	}else{
		press_button_B = UNLOCKED;
	}
	
	//At last apply the motor velocity and display it
	//Also happends in fast update
	for (u8 i=0;i<3;i++){
		motor_set_vel((MOTOR_ID)MOTOR1 + i, motor_vel[i], motor_loop_state[i]);
	}
	tft_append_line("%d %d %d", motor_vel[0], motor_vel[1], motor_vel[2]);
	tft_update();
}


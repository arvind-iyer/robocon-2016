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
static u32 curr_speed = 0;
static s16 curr_w = 0;
static s32 curr_heading = 0;

static s32 motor_vel[3] = {0};
static CLOSE_LOOP_FLAG motor_loop_state[3] = {CLOSE_LOOP};
static bool is_rotating = false;

static LOCK_STATE ground_wheels_lock = UNLOCKED;
static LOCK_STATE climbing_induced_ground_lock = UNLOCKED;
static LOCK_STATE press_button_B = UNLOCKED;
static LOCK_STATE press_button_X = UNLOCKED;
static LOCK_STATE press_button_LB = UNLOCKED;
static LOCK_STATE press_button_RB = UNLOCKED;
static LOCK_STATE press_button_back = UNLOCKED;
static LOCK_STATE press_button_start = UNLOCKED;
static LOCK_STATE press_left_joy = UNLOCKED;
static LOCK_STATE press_right_joy = UNLOCKED;

static u16 brushless_power_percent = 20;

static s16 brushless_servo_val = 0;
static u16 encoder_val = 0;

static BUTTON gripper_buttons[2] = {BUTTON_XBC_X, BUTTON_XBC_Y};
static LOCK_STATE press_gripper_buttons[2] = {UNLOCKED};
static u16 gripper_states[2] = {0};
static u32 gripper_ticks[8] = {0}; //Left claw, Left push, Right claw, Right push

static bool using_laser_sensor = false;
static bool global_axis = false;
static bool pole_front = false;
static bool rotating_machine_by_90 = false;
static s32 rotating_machine_by_90_target = 0;

static s16 last_angle_pid = 0;
static s32 sum_of_last_angle_error = 0;

static u16 accel_remainder = 0;
static u16 rotate_accel_remainder = 0;

/*
** 0 - Just started
** 1 - Following the track using laser sensors - Press back
** 2 - Disabled tracking automatically  - Use y-coordinate
** 3 - Blowing in the river (can be activated manually at any time before this stage) - Press Start
				Need to rotate brushless servo, change heading, back off, and rotate machine
** 4 - Retry, going into the pole, change heading only, machine opening face the pole - Press back and start together
*/
static u8 manual_stage = 0;

void manual_init(){
	xbc_mb_init(XBC_CAN_FIRST);
	manual_reset();
}

void manual_reset(){
	curr_vx = curr_vy = curr_angle = curr_heading = curr_w = rotating_machine_by_90_target = 0;
	manual_stage = 0;
	ground_wheels_lock = UNLOCKED;
	brushless_power_percent = 20;
	climbing_induced_ground_lock = UNLOCKED;
	press_button_B = press_button_X = press_button_back = press_left_joy = press_right_joy = UNLOCKED;
	is_rotating = using_laser_sensor = pole_front = rotating_machine_by_90 = false;
	brushless_control(0, true);
	brushless_servo_control(0);
	gripper_control(GRIPPER_1, 0); 
	gripper_control(GRIPPER_2, 0);
	pneumatic_off(&PD10);
	pneumatic_off(&PD11);
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
	s32 error = this_target_global_angle - this_curr_global_angle;
	
	if (error>1800){
		error = 3600 - error;
	}else if(error<-1800){
		error = 3600 + error;
	}
	
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


//This part deals with moving the robot around
void manual_update_wheel_base(){
	if (ground_wheels_lock == UNLOCKED){
	
		//Calcuate 3 base wheels movement
		s32 vx = 0;
		s32 vy = 0;
		//TODO: Reverse higher acceleration
		if (button_pressed(BUTTON_XBC_E) || button_pressed(BUTTON_XBC_S) || button_pressed(BUTTON_XBC_W) || button_pressed(BUTTON_XBC_N)
			|| button_pressed(BUTTON_XBC_NE) || button_pressed(BUTTON_XBC_SE) || button_pressed(BUTTON_XBC_NW) || button_pressed(BUTTON_XBC_SW)){
				
			if (button_pressed(BUTTON_XBC_N)){
				vy = 500;
			}else if (button_pressed(BUTTON_XBC_E)){
				vx = 500;
			}else if (button_pressed(BUTTON_XBC_S)){
				vy = -500;
			}else if (button_pressed(BUTTON_XBC_W)){
				vx = -500;
			}else if (button_pressed(BUTTON_XBC_NE)){
				vx = 353;
				vy = 353;
			}else if (button_pressed(BUTTON_XBC_SE)){
				vx = 353;
				vy = -353;
			}else if (button_pressed(BUTTON_XBC_SW)){
				vx = -353;
				vy = -353;
			}else if (button_pressed(BUTTON_XBC_NW)){
				vx = -353;
				vy = 353;
			}
			
			global_axis = true;
			
		}else{
			vx = xbc_get_joy(XBC_JOY_LX);
			vy = xbc_get_joy(XBC_JOY_LY);
			
			if (vx!=0 || vy!=0){
				global_axis = false;
			}
		}
		
		if (climbing_induced_ground_lock == UNLOCKED){
			/*
			** This part calculate acceleration
			** It caps the max. acceleration for both x and y
			** By finding the axis with more difference(thus more acceleration), cap it
			** Then use the same proportion for the another axis to ensure the angle is correct
			*/
			
			u16 acceleration_amount;
			if (abs(vx)<10 && abs(vy)<10){
				acceleration_amount = BASE_ACCEL_CONSTANT + accel_remainder; //Scaled by 1000
			}else{
				s32 target_angle = int_arc_tan2(vx, vy);
				s32 last_angle = int_arc_tan2(curr_vx, curr_vy);
				s32 angle_diff = abs(target_angle - last_angle);
				acceleration_amount = BASE_ACCEL_CONSTANT*(angle_diff+90)/90 + accel_remainder; //Scaled by 1000
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
			
			if (global_axis){
				curr_angle = int_arc_tan2(curr_vx, curr_vy)*10 - get_angle();
			}else{
				curr_angle = int_arc_tan2(curr_vx, curr_vy)*10;
			}
			
			if (pole_front){
				curr_angle -= 1800;
			}
			
			s32 w = (xbc_get_joy(XBC_JOY_LT)-xbc_get_joy(XBC_JOY_RT))*8/5;
			if (Abs(w-curr_w) < 5){
				curr_w = w;
			}else{
				s32 rotate_accel_amount = ROTATE_ACCEL_CONSTANT + rotate_accel_remainder;
				rotate_accel_remainder = rotate_accel_amount % 1000;
				rotate_accel_amount /= 1000;
				
				if (w > curr_w){
					curr_w += rotate_accel_amount;
				}else{
					curr_w -= rotate_accel_amount;
				}
			}
			
			//change heading for angle PID use
			if (curr_w == 0){
				if (is_rotating){
					is_rotating = false;
					curr_heading = get_angle();
				}
			}else{
				is_rotating = true;
			}

			curr_speed = u32_sqrt((u32)(curr_vx*curr_vx + curr_vy*curr_vy));
			motor_vel[0] = (int_sin(curr_angle%3600)*(s32)curr_speed*(-1)/10000 + curr_w)/10;
			motor_vel[1] = (int_sin((curr_angle+1200)%3600)*(s32)curr_speed*(-1)/10000 + curr_w)/10;
			motor_vel[2] = (int_sin((curr_angle+2400)%3600)*(s32)curr_speed*(-1)/10000 + curr_w)/10;
			
			s16 motor_vel_max = motor_vel[0];
			for (u8 i=0; i<3; i++){
				if (abs(motor_vel[i])>abs(motor_vel_max)){
					motor_vel_max = motor_vel[i];
				}
			}
			
			if (abs(motor_vel_max)>150){
				s32 motor_ratio = 150*10000/abs(motor_vel_max); //Scaled by 10000
				for (u8 i=0;i<3;i++){
					motor_vel[i] = (s32)motor_vel[i]*motor_ratio/10000;
				}
			}
			
			for (u8 i=0;i<3;i++){
				motor_loop_state[i] = CLOSE_LOOP;
			}
		}else{
			for (u8 i=0;i<3;i++){
				motor_vel[i] = 0;
				motor_loop_state[i] = OPEN_LOOP;
			}
		}
	}
}

/*
** This fast update is for controlling things that require a high refresh rate
** It contains:
** - Locking itself in place
** - Angle PID (disabled)
** - Laser tracking
*/
void manual_fast_update(){
	s32 curr_rotate = 0;
	
	for (u8 i=0;i<3;i++){
		motor_vel[i] = 0;
	}
	
	if (!gpio_read_input(&ARM_IR_PORT)){
		raise_arm();
	}
	
	if(manual_stage==1 && using_laser_sensor){
		laser_manual_update(motor_vel, &curr_rotate);
		curr_heading = get_angle();
	}else if (rotating_machine_by_90){
		curr_rotate += river_rotate_update(rotating_machine_by_90_target);
		if (abs(curr_rotate) < 5){
			rotating_machine_by_90 = false;
		}
		curr_heading = get_angle();
	}else if (!using_laser_sensor){
		manual_update_wheel_base();
		if (!is_rotating){
			//curr_rotate = -angle_pid()/1000;
		}
	}
	
	s16 motor_vel_max = motor_vel[0];
	for (u8 i=1; i<3; i++){
		if (abs(motor_vel[i])>abs(motor_vel_max)){
			motor_vel_max = motor_vel[i];
		}
	}
	
	if (abs(motor_vel_max)>150){
		s32 motor_ratio = 150*10000/abs(motor_vel_max); //Scaled by 10000
		for (u8 i=0;i<3;i++){
			motor_vel[i] = (s32)motor_vel[i]*motor_ratio/10000;
		}
	}
	
	for (u8 i=0;i<3;i++){
		motor_vel[i] += curr_rotate/10;
		motor_loop_state[i] = CLOSE_LOOP;
		motor_set_vel((MOTOR_ID)MOTOR1 + i, motor_vel[i], motor_loop_state[i]);
	}
}

//Interval update is called in a timed interval, reducing stress
void manual_interval_update(){
	tft_clear();
	
	tft_append_line("%d", this_loop_ticks);
	tft_append_line("%d", this_loop_ticks-last_long_loop_ticks);
	
	//If Y is too large, disable the laser tracking 
	#ifdef RED_FIELD
	if ((-get_pos()->x) > LASER_TRACING_OFF_DISTANCE){
	#else
	if (get_pos()->x > LASER_TRACING_OFF_DISTANCE){
	#endif
		if (manual_stage<2){
			manual_stage = 2;
			using_laser_sensor = false;
		}
	}
	
	//Pressing the back button to enable laser tracking
	if (button_pressed(BUTTON_XBC_BACK)){
		if (press_button_back == UNLOCKED){
			press_button_back = LOCKED;
			buzzer_beep(75);
			if (manual_stage < 2){
				using_laser_sensor = !using_laser_sensor;
				manual_stage = 1;
			}
		}
	}else{
			press_button_back = UNLOCKED;
	}
	
	//Pressing the start button for shooting across the river
	if (button_pressed(BUTTON_XBC_START)){
		if (press_button_start == UNLOCKED){
			press_button_start = LOCKED;
			buzzer_beep(75);
			if (manual_stage < 3){
				pole_front = true;
				rotating_machine_by_90 = true;
				using_laser_sensor = false;
				#ifdef BLUE_FIELD
					rotating_machine_by_90_target = (get_angle() - 900)%3600;
					brushless_servo_val = 90;
				#else
					rotating_machine_by_90_target = (get_angle() + 900)%3600;
					brushless_servo_val = -90;
				#endif
				manual_stage = 3;
				brushless_servo_control(brushless_servo_val);
			}
		}
	}else{
			press_button_start = UNLOCKED;
	}
	
	//At last apply the motor velocity and display it
	//Value from fast update should also be displayed here
	tft_append_line("%d", curr_speed);
	tft_append_line("%d %d %d", get_pos()->x, get_pos()->y, get_angle());
	tft_append_line("ST: %d %d", using_laser_sensor, manual_stage);
	tft_append_line("%d %d %d", motor_vel[0], motor_vel[1], motor_vel[2]);
	tft_update();
}

void manual_controls_update(void) {
	//Brushless power control
	if (button_pressed(BUTTON_XBC_RB)){
		if (press_button_RB == UNLOCKED){
			press_button_RB = LOCKED;
			buzzer_beep(75);
			if (brushless_power_percent < 100){
				brushless_power_percent += BRUSHLESS_POWER_STEP;
			}
			if (brushless_power_percent == (20 + BRUSHLESS_POWER_STEP)){
				brushless_power_percent += BRUSHLESS_POWER_STEP;
			}
		}
	}else{
			press_button_RB = UNLOCKED;
	}
	
	if (button_pressed(BUTTON_XBC_LB)){
		if (press_button_LB == UNLOCKED){
			press_button_LB = LOCKED;
			buzzer_beep(75);
			if (brushless_power_percent > 20){
				brushless_power_percent -= BRUSHLESS_POWER_STEP;
			}
		}
	}else{
			press_button_LB = UNLOCKED;
	}
	
	bool both_joy_pressed = true;
	
	if (button_pressed(BUTTON_XBC_L_JOY)){
		if (press_left_joy == UNLOCKED){
			press_left_joy = LOCKED;
			pole_front = !pole_front;
		}
	}else{
		press_left_joy = UNLOCKED;
	}
	
	brushless_control(brushless_power_percent, true);
	tft_append_line("%d", brushless_power_percent);
	
	// brushless arm
	if (xbc_get_joy(XBC_JOY_RX)>500){
		brushless_servo_val += BRUSHLESS_SERVO_STEP;
		if (brushless_servo_val > 140)
			brushless_servo_val = 140;
		brushless_servo_control(brushless_servo_val);
	}
	if (xbc_get_joy(XBC_JOY_RX)<-500){
		brushless_servo_val -= BRUSHLESS_SERVO_STEP;
		if (brushless_servo_val < -140)
			brushless_servo_val = -140;
		brushless_servo_control(brushless_servo_val);
	}
	
	if (xbc_get_joy(XBC_JOY_RY)>700){
		raise_arm();
		tft_append_line("RAISING ARM");
	}else if (xbc_get_joy(XBC_JOY_RY)<-700){
		lower_arm();
		tft_append_line("LOWERING ARM");
	}else {
		stop_arm();
	}
	
	
	//encoder_val = get_encoder_count(ENCODER1);
	
	//gripper controls
	for (u8 i=0; i < GRIPPER_COUNT; i++) {
		if (button_pressed(gripper_buttons[i])) {
			if (press_gripper_buttons[i] == UNLOCKED) {
				press_gripper_buttons[i] = LOCKED;
				gripper_states[i] = (gripper_states[i] + 1) % GRIPPER_STATES_NO;
				
				switch (gripper_states[i]) {
					case 0: //Default state
						gripper_control((GRIPPER_ID)i, 0); //Down
						gripper_ticks[3+i*4] = get_full_ticks() + 1000; //pneumatic 2 hi; pushed out
						break;
					case 1: //Collect propeller
						gripper_ticks[0+i*4] = get_full_ticks() + 200; //pneumatic 1 lo; close claw
						gripper_ticks[2+i*4] = get_full_ticks() + 500; //pneumatic 2 lo; retract
						break;
					case 2: //Upright
						gripper_control((GRIPPER_ID)i, 1); //Up
						gripper_ticks[3+i*4] = get_full_ticks() + 1800; //pneumatic 2 hi; pushed out 
						break;
					case 3: //Chaiyo
						gripper_ticks[2+i*4] = get_full_ticks() + 200; //pneumatic 2 lo; retract
						gripper_ticks[1+i*4] = get_full_ticks() + 800; //pneumatic 1 hi; claw open 
						break;
				}
			}
		}else{
			press_gripper_buttons[i] = UNLOCKED;
		}
	}
	
	for (u8 i=0; i<8; i++) {
		if(Abs(get_full_ticks() - gripper_ticks[i]) <= GRIPPER_TICKS_THRESHOLD) {
			GPIO gripper_ticks_port;
			switch ((int)(i/2)) {
				case 0: //left claw
					gripper_ticks_port = PD10;
					break;
				case 1: //left push
					gripper_ticks_port = PD11;
					break;
				case 2: //right claw
					gripper_ticks_port = PD8;
					break;
				case 3: //right push
					gripper_ticks_port = PD9;
					break;
			}
			if (i%2) {
				pneumatic_off(&gripper_ticks_port);
			} else {
				pneumatic_on(&gripper_ticks_port);
			}
		}
	}
	
	/*
	** This part is related to climbing mechanism
	** Hold button A to climb up
	** Hold button Y to descend
	** While climbing, the ground wheels should be locked
	*/
	if (button_pressed(BUTTON_XBC_A) && gpio_read_input(&PE3)){
		climb_continue();
		climbing_induced_ground_lock = LOCKED;
		tft_append_line("CLIMBING");
	}else{
		stop_climbing();
		climbing_induced_ground_lock = UNLOCKED;
	}
	
	/*
	** This part deals with locking the robot onto the pole.
	*/
	if (button_pressed(BUTTON_XBC_B)){
		if (press_button_B == UNLOCKED){
			press_button_B = LOCKED;
			pneumatic_climb_toggle();
		}
	}else{
		press_button_B = UNLOCKED;
	}
}

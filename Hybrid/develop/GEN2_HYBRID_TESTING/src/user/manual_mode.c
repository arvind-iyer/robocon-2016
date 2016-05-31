#include "manual_mode.h"

/**
* Control Scheme:
* Left joy stick for ground wheels movement
* Right joy stick for moving brushless arm and servo
* LB and RB for controlling brushless power, press once to inc./dec.
* LT and RT for rotation
*
* X to switch mode 0 <---> 1, rotate by 90, perpare gribber
* Back to activate auto go into pole mode
*
* In mode 0:
* Y - Activate/Deactivate laser tracking
* B - Rotating brushless servo by 90
*
* In mode 1:
* A - Claw pneumatic control
* B - Push pneumatic control
* Y - Whole gibber servo control
*
* The arm should rise automatically using IR at all times.
* Angel PID is disabled.
*/

static s32 curr_vx = 0, curr_vy = 0, curr_angle = 0;
static u32 curr_speed = 0;
static s16 curr_w = 0;
static s32 curr_heading = 0;

static s32 motor_vel[3] = {0};
static CLOSE_LOOP_FLAG motor_loop_state[3] = {CLOSE_LOOP};
static bool is_rotating = false;

static LOCK_STATE ground_wheels_lock = UNLOCKED;
static LOCK_STATE climbing_induced_ground_lock = UNLOCKED;

static u16 brushless_power_percent = 20;

static s16 brushless_servo_val = 0;
static u16 encoder_val = 0;

static bool using_laser_sensor = false;
static bool pole_as_front = false;
static bool facing_pole = false;
static bool brushless_str = false;
static bool rotating_machine_by_90 = false;
static s32 rotating_machine_by_90_target = 0;

static bool climbing_pneumatic_on = false;
static s32 climbing_pneumatic_ticks = 0;
//delay->push->delay->unclaw->delay->retract->delay->beep
static u8 putting_propeller_state = 0;
static s32 putting_propeller_ticks[4] = {0};

static bool gripper_extended = false;
static bool gripper_clawed = true;
static bool gripper_down = false;

static bool global_axis = false;

static s16 last_angle_pid = 0;
static s32 sum_of_last_angle_error = 0;

static u16 accel_remainder = 0;
static u16 rotate_accel_remainder = 0;

/*
** 0 - Blowing mode
** 1 - Grabbing mode
** 2 - Auto go into pole mode
** 3 - Climb mode
** 4 - Putting propeller
** 5 - Semi-auto approach wall
** 6 - Semi-auto calibrate gyro and xy reading
** 7 - Semi-auto get propeller
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
	is_rotating = using_laser_sensor = pole_as_front = facing_pole = brushless_str = rotating_machine_by_90 = false;
	gripper_extended = gripper_down = climbing_pneumatic_on = false;
	gripper_clawed = true;
	brushless_control(0, true);
	brushless_servo_control(0);
	brushless_servo_val = 0;
	for (GRIPPER_ID id=GRIPPER_1;id<=GRIPPER_2;id++){
		gripper_control(id, gripper_down);
		gripper_push_control(id, gripper_extended);
		gripper_claw_control(id, gripper_clawed);
	}
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
				vy = 800;
			}else if (button_pressed(BUTTON_XBC_E)){
				vx = 800;
			}else if (button_pressed(BUTTON_XBC_S)){
				vy = -800;
			}else if (button_pressed(BUTTON_XBC_W)){
				vx = -800;
			}else if (button_pressed(BUTTON_XBC_NE)){
				vx = 566;
				vy = 566;
			}else if (button_pressed(BUTTON_XBC_SE)){
				vx = 566;
				vy = -566;
			}else if (button_pressed(BUTTON_XBC_SW)){
				vx = -566;
				vy = -566;
			}else if (button_pressed(BUTTON_XBC_NW)){
				vx = -566;
				vy = 566;
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
			
			if (pole_as_front){
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
	
	if (manual_stage == 5){
		manual_stage = limit_sa_approach(motor_vel, &curr_rotate);		
	}else if (manual_stage == 2){
		raise_arm();
		manual_stage = limit_manual_update(motor_vel, &curr_rotate);		
		curr_heading = get_angle();		
	}else if (manual_stage == 6){
		raise_arm();
		manual_stage = limit_sa_update(motor_vel, &curr_rotate);
		curr_heading = get_angle();
	}else if(manual_stage == 7){
		manual_stage = sa_str_update(motor_vel, &curr_rotate);
		curr_heading = get_angle();
	}else if(manual_stage == 3){
		raise_arm();
		if (!climbing_pneumatic_on){
			pneumatic_off(&CLIMB_PNEUMATIC_PORT);
			climbing_pneumatic_on = true;
			climbing_pneumatic_ticks = this_loop_ticks;
			gripper_extended = false;
			gripper_push_control(THIS_GRIPPER, gripper_extended);
			gripper_down = false;
			gripper_control(THIS_GRIPPER, gripper_down);
		}else if((get_full_ticks() - climbing_pneumatic_ticks)>CLIMBING_TICKS_LIMIT){
			climb_continue();
			brushless_servo_val = 0;
			brushless_servo_control(brushless_servo_val);
			brushless_control(CLIMBING_BRUSHLESS_POWER, true);
			if (!gpio_read_input(&HIT_BOX_PORT)){
				manual_stage++;
				putting_propeller_ticks[0] = this_loop_ticks;
				putting_propeller_state = 0;
			}
		}
	}else if(manual_stage == 4){
		if (putting_propeller_state==0 && ((putting_propeller_ticks[0] - this_loop_ticks) > PUTTING_PROPELLER_PUSH_DELAY)){
			putting_propeller_state = 1;
			gripper_extended = true;
			gripper_push_control(THIS_GRIPPER, gripper_extended);
			putting_propeller_ticks[1] = this_loop_ticks;
		}else	if (putting_propeller_state==1 && ((putting_propeller_ticks[1] - this_loop_ticks) > PUTTING_PROPELLER_UNCLAW_DELAY)){
			gripper_clawed = false;
			gripper_claw_control(THIS_GRIPPER, gripper_clawed);
			putting_propeller_state = 2;
			putting_propeller_ticks[2] = this_loop_ticks;
		}else	if (putting_propeller_state==2 && ((putting_propeller_ticks[2] - this_loop_ticks) > PUTTING_PROPELLER_RETRACT_DELAY)){
			gripper_extended = false;
			gripper_push_control(THIS_GRIPPER, gripper_extended);
			putting_propeller_state = 3;
			putting_propeller_ticks[3] = this_loop_ticks;
		}else	if (putting_propeller_state==3 && ((putting_propeller_ticks[3] - this_loop_ticks) > PUTTING_PROPELLER_YEAH_DELAY)){
			putting_propeller_state = 4;
		}
		
	}else if(manual_stage == 0 && using_laser_sensor){
		using_laser_sensor = laser_manual_update(motor_vel, &curr_rotate);
		curr_heading = get_angle();
	}else if (rotating_machine_by_90){
		s32 this_rotate = river_rotate_update(rotating_machine_by_90_target);
		for (u8 i=0;i<3;i++){
			motor_vel[i] += this_rotate/10;
		}
		if (abs(rotating_machine_by_90_target - get_angle())%3600 < 50){
			rotating_machine_by_90 = false;
		}
		curr_heading = get_angle();
	}else if (!using_laser_sensor){
		manual_update_wheel_base();
		if (!is_rotating){
			//curr_rotate = -angle_pid()/1000;
		}
	}
	
	for (u8 i=0;i<3;i++){
		motor_loop_state[i] = CLOSE_LOOP;
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
		motor_set_vel((MOTOR_ID)MOTOR1 + i, motor_vel[i], motor_loop_state[i]);
	}
}

//Interval update is called in a timed interval, reducing stress
void manual_interval_update(){
	tft_clear();
	
	tft_append_line("%d", this_loop_ticks);
	tft_append_line("%d", this_loop_ticks-last_long_loop_ticks);
	sa_print_info();
	
	if (button_hitted[BUTTON_XBC_START]){
		buzzer_beep(75);
		if (manual_stage == 0){
			pole_as_front = true;
			using_laser_sensor = false;
			manual_stage = 1;
		}else if(manual_stage == 1){
			manual_stage = 0;
		}
	}
	
	if (button_hitted[BUTTON_XBC_X]){
		#ifdef BLUE_FIELD
			if (!facing_pole){
		#else
			if (facing_pole){
		#endif
				rotating_machine_by_90_target = (get_angle() - 900)%3600;
				manual_stage = 1;
				gripper_down = true;
				gripper_extended = true;
				gripper_clawed = false;
				gripper_control(THIS_GRIPPER, gripper_down);
				gripper_push_control(THIS_GRIPPER, gripper_extended);
				gripper_claw_control(THIS_GRIPPER, gripper_clawed);
			}else{
				rotating_machine_by_90_target = (get_angle() + 900)%3600;
				manual_stage = 0;
				gripper_down = false;
				gripper_extended = false;
				gripper_clawed = true;
				gripper_control(THIS_GRIPPER, gripper_down);
				gripper_push_control(THIS_GRIPPER, gripper_extended);
				gripper_claw_control(THIS_GRIPPER, gripper_clawed);
			}
			rotating_machine_by_90 = true;
			using_laser_sensor = false;
			pole_as_front = !pole_as_front;
			facing_pole = !facing_pole;
	}
			
	if (button_hitted[BUTTON_XBC_A]){
		if (manual_stage == 0){
			manual_stage = 5;
			limit_sa_approach_init();
			gripper_down = true;
			gripper_extended = true;
			gripper_clawed = false;
			gripper_control(THIS_GRIPPER, gripper_down);
			gripper_push_control(THIS_GRIPPER, gripper_extended);
			gripper_claw_control(THIS_GRIPPER, gripper_clawed);
			using_laser_sensor = false;
			pole_as_front = true;
		}else{
			manual_stage = 1;
			gripper_down = true;
			gripper_extended = true;
			gripper_clawed = false;
			gripper_control(THIS_GRIPPER, gripper_down);
			gripper_push_control(THIS_GRIPPER, gripper_extended);
			gripper_claw_control(THIS_GRIPPER, gripper_clawed);
			using_laser_sensor = false;
			pole_as_front = true;
		}
	}
	
	if (button_hitted[BUTTON_XBC_BACK]){
		buzzer_beep(175);
		limit_manual_init();
		manual_stage = 2;
		gripper_down = true;
		gripper_control(THIS_GRIPPER, gripper_down);
	}
	
	if (manual_stage == 0){
		manual_first_control_update();
	}else if(manual_stage == 1){
		manual_second_control_update();
	}
	
	//At last apply the motor velocity and display it
	//Value from fast update should also be displayed here
	tft_append_line("%d", curr_speed);
	tft_append_line("%d %d %d", get_pos()->x, get_pos()->y, get_angle());
	tft_append_line("%d %d %d %d %d %d", using_laser_sensor, manual_stage, facing_pole, brushless_str, rotating_machine_by_90, rotating_machine_by_90_target);
	tft_append_line("LS: %d %d", get_ls_cal_reading(0), get_ls_cal_reading(2));
	tft_append_line("%d %d %d", motor_vel[0], motor_vel[1], motor_vel[2]);
	tft_update();
}

void manual_first_control_update(){
	manual_control_brushless_update(); 
	
	if (button_hitted[BUTTON_XBC_B]){
		if (brushless_str){
			#ifdef BLUE_FIELD
				brushless_servo_val = 90;
			#else
				brushless_servo_val = -90;
			#endif
		}else{
			brushless_servo_val = 0;
		}
		brushless_str = !brushless_str;
		brushless_servo_control(brushless_servo_val);
	}
	
	//Pressing the back button to enable laser tracking
	if (button_hitted[BUTTON_XBC_Y]){
		buzzer_beep(75);
		using_laser_sensor = !using_laser_sensor;
	}
}

void manual_second_control_update() {
	manual_control_brushless_update();
	
	//Gripper servo
	if (button_hitted[BUTTON_XBC_Y]){
		gripper_down = !gripper_down;
		gripper_control(THIS_GRIPPER, gripper_down);
	}
	//Gripper push
	if (button_hitted[BUTTON_XBC_B]){
		gripper_extended = !gripper_extended;
		gripper_push_control(THIS_GRIPPER, gripper_extended);
	}
	//Gripper claw
	if (button_hitted[BUTTON_XBC_A]){
		gripper_clawed = !gripper_clawed;
		gripper_claw_control(THIS_GRIPPER, gripper_clawed);
	}
}

void manual_control_brushless_update(){
	//Brushless power control
	if (button_hitted[BUTTON_XBC_RB]){
		buzzer_beep(75);
		if (brushless_power_percent < 100){
			brushless_power_percent += BRUSHLESS_POWER_STEP;
		}
		if (brushless_power_percent == (20 + BRUSHLESS_POWER_STEP)){
			brushless_power_percent += BRUSHLESS_POWER_STEP;
		}
	}
	
	if (button_hitted[BUTTON_XBC_LB]){
		buzzer_beep(75);
		if (brushless_power_percent > 20){
			brushless_power_percent -= BRUSHLESS_POWER_STEP;
		}
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
}


void manual_controls_update() {
	manual_control_brushless_update();
}

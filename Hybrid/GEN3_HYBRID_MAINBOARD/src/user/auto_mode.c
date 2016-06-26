/**
  ******************************************************************************
  * _____  ________________   ________      ______                           
  * __  / / /_  ___/__  __/   ___  __ \________  /__________________________ 
  * _  / / /_____ \__  /      __  /_/ /  __ \_  __ \  __ \  ___/  __ \_  __ \
  * / /_/ / ____/ /_  /       _  _, _// /_/ /  /_/ / /_/ / /__ / /_/ /  / / /
  * \____/  /____/ /_/        /_/ |_| \____//_.___/\____/\___/ \____//_/ /_/ 
  *                                                                
  * @file    auto_mode.c
  * @author  Pang Hong Wing
  * @version V0.3.0
  * @date    25 Feb, 2016
  * @brief   Library for hybrid auto movement mode, in particular for PID
  *          movement tracking.
  ******************************************************************************
  */

#include "auto_mode.h"

#define THRESHOLD 10
#define KP 0.3
#define KI 0.015
#define RKP 1.8
#define DEC_COEFF 8.0
//#define WALL_CAL 4400
#define WALL_CAL 6600
#define ARM_SPEED 1500
#define LS_DIFF 400
#define SHIFT 3.0
#define INNER_DIST 280
#define OUTER_DIST 310
#define RETRY_STATES_NO 3

const SERVO_ID gripper_servo[2] = {SERVO2, SERVO1};
const GPIO * gripper_push[2] = {&PD11, &PD9};
const GPIO * gripper_claw[2] = {&PD10, &PD8};
const GPIO * pole_clamp = &PB9;
const u16 servo_up_val[2] = {870, 1035};
const u16 servo_dn_val[2] = {1078, 645};

//#define DEBUG_MODE

//Ground: 0 = Red, 1 = Blue
#ifdef RED_FIELD
	u8 field = 0;
#else
	u8 field = 1;
#endif

double transform[2][2] = {{1, 0}, {0, 1}};
u16 wall_dist = 0;
u8 hill_cal = 1;

//mode variables
PID_MODE pid_state;
bool pid_stopped;
bool start_pressed, back_pressed, y_pressed;
int path_id;

//path target queue
TARGET tar_queue[50];
s32 tar_head, tar_end;

//path target variables
s32 tar_x, tar_y, tar_deg, tar_dir;
s32 tar_rad, tar_cen_x, tar_cen_y;
s32 ori_x, ori_y;
s32 off_x, off_y, off_deg;
double deg_ratio;

//auto properties
s32 raw_x, raw_y;
s32 cur_x, cur_y, cur_deg;
int vel[3];
int degree, degree_diff, dist, speed;
int dist_last, time, time_last, measured_vel;
int start, passed;
int err_d, err_sum;
int auto_ticks = 0;
s16 cur_vel = 0;

u8 side_switch_val = 0;

//Robot control
bool arm_init = false;
s16 arm_vel = 0;
s16 tar_arm = 0;
s16 brushless_time = 0;
s16 climbing_time = 0;
s16 arrived_time = 0;
s32 top_time = 0;
s16 climb_dir = 0;
s16 climb_blow_pwm = 0;
bool arrived = false;
bool at_top = false;
u8 climb_temp = 0;
double climb_speed = 1;
u8 retry_state = NO_RETRY;
u8 semi_auto_state = 0;
s32 pause_time = 0;
u8 ir_last = 0;
u8 ir_now = 0;

//UART receiver
u8 rx_state = 0;
uint8_t rx_buffer[4] = {0,0,0,0};

u8 rx_path_length = 0;
u8 rx_count = 0;
u8 rx_pointer = 0;
u8 rx_node_no = 0;
u32 rx_node_list[30][NODE_SIZE];
bool to_be_saved = false;

s32 rx_merge(void) {
	s32 val = 0;
	val |= rx_buffer[0] << 0;
	val |= rx_buffer[1] << 8;
	val |= rx_buffer[2] << 16;
	val |= rx_buffer[3] << 24;
	return val;
}

int pythag(int a1, int a2, int b1, int b2) {
	return Sqrt(Sqr(b1 - a1) + Sqr(b2 - a2));
}

/**
  * @brief  Add target to queue
  * @param  target: struct to be added
  * @retval None
  */
void auto_tar_enqueue(TARGET target) {	
	tar_queue[tar_head] = target;
	tar_head++;
}

/**
  * @brief  Execute topmost target in queue
  * @param  None
  * @retval None
  */
void auto_tar_dequeue() {
	int mid_length;
	if (tar_end && (tar_queue[tar_end-1].type == NODE_STOP))
		start = auto_get_ticks();
	brushless_time = auto_get_ticks();
	arrived = false;
	
	if (tar_end) {
		ori_x = tar_queue[tar_end-1].x;
		ori_y = tar_queue[tar_end-1].y;
	} else {
		ori_x = 0;
		ori_y = 0;
	}
	
	//speed control - notice tar_end needs to minus one
	if (!retry_state) {
		if (tar_end == 1)
			cur_vel = 65;
		if (tar_end == 2)
			cur_vel = 50;
	} else if (retry_state == RETRY_HILL) {
		if (tar_end == 2)
			cur_vel = 80;
		if (tar_end == 3)
			cur_vel = 50;
	}
	if (tar_end == 4)
		cur_vel = 6;
	if (tar_end == 5)
		cur_vel = 70;	
	if (tar_end == 9)
		cur_vel = 40;	
	tar_x = tar_queue[tar_end].x;
	tar_y = tar_queue[tar_end].y;
	//tar_deg = tar_queue[tar_end].deg;
	tar_dir = 90 - int_arc_tan2(tar_y - ori_y, tar_x - ori_x);
	
	tar_rad = 0;
	if (tar_queue[tar_end].curve)
		tar_rad = 1000000/tar_queue[tar_end].curve;
	
	mid_length = Sqr(tar_x - ori_x) + Sqr(tar_y - ori_y);
	mid_length = Sqrt(Sqr(tar_rad) - mid_length/4);
	mid_length *= (tar_rad / Abs(tar_rad));
	tar_cen_x = (ori_x + tar_x)/2 + mid_length * int_cos(tar_dir * 10) / 10000;
	tar_cen_y = (ori_y + tar_y)/2 - mid_length * int_sin(tar_dir * 10) / 10000;
	
	err_sum = 0;
	tar_end++;
}

/**
  * @brief	Return target length
  * @param  None
  * @retval Length of target queue
  */
int auto_tar_queue_len() {
	return tar_head - tar_end;
}

TARGET auto_tar_ret(int id) {
	if (id < 0) {
		TARGET null;
		null.type = NODE_STOP;
		null.x = 0;
		null.y = 0;
		null.deg = 0;
		null.curve = 0;
		return null;
	} else {
		return tar_queue[id];
	}
}

/**
  * @brief  Calculate ticks offset by a value to be used in auto mode only
  * @param  None
  * @retval No. of ticks in auto mode
  */
int auto_get_ticks(){
	return get_full_ticks() - auto_ticks;
}

/**
  * @brief  Initialize menu of auto mode
  * @param  None
  * @retval None
  */
void auto_init() {
	pid_state = MENU_MODE;
	path_id = 0;

	start_pressed = false;
	back_pressed = false;
	y_pressed = false;
	
	tar_head = 0;
	tar_end = 0;
}

/**
  * @brief  Reset running variables before starting PID
  * @param  None
  * @retval None
  */
void auto_reset() {
	//reset variables
	err_d = 0;
	err_sum = 0;
	
	dist = 0;
	dist_last = 0;
	time = 0;
	time_last = 0;
	degree_diff = 0;
	
	tar_x = 0;
	tar_y = 0;
	tar_deg = 0;
	
	off_x = 0;
	off_y = 0;
	off_deg = 0;
	
	deg_ratio = 0;
	start = 0;
	cur_vel = 100; //initial vel
	pid_stopped = false;
	hill_cal = 1;
	
	if (!semi_auto_state)
		transform[1][0] = 0;
	
	tar_arm = 0;
	brushless_time = 0;
	climbing_time = 0;
	top_time = 0;
	pneumatic_off(pole_clamp); //Open wheels
	pneumatic_on(gripper_claw[field]); //Open claw
	pneumatic_on(gripper_push[field]); //Push out
	servo_control(gripper_servo[field], servo_dn_val[field]);
	climb_dir = 0;
	climb_blow_pwm = 0;
	at_top = false;
	pause_time = 0;
	ir_last = 1;
	ir_now = 1;
	
	//reset local timer
	auto_ticks = get_full_ticks();
	
	//reset gyro location
	if (!semi_auto_state)
		gyro_pos_set(0,0,0);
	
	//dequeue first target
	auto_tar_dequeue();
}

/**
  * @brief  Return state of hybrid auto mode
  * @param  None
  * @retval True if hybrid is in PID mode
  */
PID_MODE auto_get_state() {
	return pid_state;
}

/**
  * @brief  Fetch data stored in flash memory
  * @param  page: 0 for header, 1 for path data
  * @param  offset: header - 0 for start flag, 1 for length; data - no. of 32bit val
  * @retval Requested value in u32 (regardless of 16 / 32 bit)
  */
u32 auto_get_flash(u8 page, u8 offset) {
	if (page == 0) 
		return *((uint16_t *)(HEADER_BASE_ADDR + offset*2));
	if (page == 1) 
		return *((uint32_t *)(PATH_BASE_ADDR + offset*4));
}

/**
  * @brief  Return state of hybrid auto mode
  * @param  angle: relative direction of straight path, overwritten if curve
  * @param  rotate: relative angle of target to current location
  * @param  maxvel: velocity of hybrid
  * @param  curved: curved path if true
  * @retval None
  */
void auto_track_path(int angle, int rotate, int maxvel, bool curved) {
	int p, q;
	int err, err_pid;
	double dotcheck;
	
	double kp = KP;
	double ki = KI;
	
	//determine if overflow
	dotcheck = (cur_x-tar_x)*(ori_x-tar_x) + (cur_y-tar_y)*(ori_y-tar_y);
	dotcheck /= Sqrt(Sqr(ori_x-tar_x)+Sqr(ori_y-tar_y));
	dotcheck /= Sqrt(Sqr(cur_x-tar_x)+Sqr(cur_y-tar_y));
	if (dotcheck <= 0.0) {
		if (auto_tar_queue_len() && (tar_queue[tar_end-1].type == NODE_PASS))
			auto_tar_dequeue();
		else
			angle = 90 - int_arc_tan2(tar_y - cur_y, tar_x - cur_x) - (int)(cur_deg/10);
	}
	
	//determine velocity coefficient
	double acc, dec;
	if (passed < 1200)
		acc = int_sin((passed-600)*1.5)/20000.0+0.5;
	else
		acc = 1.0;
	if ((dist < 960) && (tar_queue[tar_end-1].type == NODE_STOP)) {
		if (dotcheck <= 0.0)
			dec = dist/250.0;
		else
			dec = sqrt(dist/(cur_vel*13.6));
	}	else {
		dec = 1.0;
	}
	//double dec = dist / ((double)(cur_vel * DEC_COEFF));
	double vel_coeff = acc < dec ? acc : dec;
	if (vel_coeff > 1.0)
		vel_coeff = 1.0;
	
	//find perpendicular dist
	if (tar_queue[tar_end-1].curve == 0) { //straight line
		p = tar_x - ori_x;
		q = tar_y - ori_y;
		err = p*(tar_y - cur_y) - q*(tar_x - cur_x);
		err /= (int)(Sqrt(Sqr(p)+Sqr(q)));
	} else { //curve
		err = Sqrt(Sqr(tar_cen_x - cur_x) + Sqr(tar_cen_y - cur_y));
		err *= (tar_rad / Abs(tar_rad));
		err = tar_rad - err;
	}
	
	//limit switches
	s8 reset_rot = 0;
	s8 reset_vel[3] = {0, 0, 0};
	
	if (field == 0)
		side_switch_val = ((cur_x >= 0) || (cur_x <= -12900))*4 + gpio_read_input(&PE11)*2 + gpio_read_input(&PE10);
	if (field == 1)
		side_switch_val = ((cur_x <= 0) || (cur_x >= 12900))*4 + gpio_read_input(&PE1)*2 + gpio_read_input(&PE0);
	
	if ((side_switch_val == 3) || (side_switch_val & 4)) {
		if (Abs(cur_x) < 7000) {
			off_x = raw_x;
		} else {
			if (field == 0)
				off_x = raw_x + 12900;
			if (field == 1)
				off_x = raw_x - 12900;
		}
		if ((side_switch_val == 3) || (side_switch_val == 7)) {
			if (Abs(cur_x) < 7000) {
				off_deg = get_angle();
			} else {
				off_deg = get_angle() - 1800;
				if (tar_end == 8) //directly dequeue if touch wall before 12900
					auto_tar_dequeue();
			}
			err_sum = 0;
		}
	}
	
	if ((side_switch_val & 2) && !(side_switch_val & 1)) {
		reset_rot = cur_vel/25;
	}
	if (!(side_switch_val & 2) && (side_switch_val & 1)) {
		reset_rot = cur_vel/(-25);		
	}
	
	if (side_switch_val == 4) {
		if (field == 0) {
			reset_vel[0] = cur_vel/(-5);
			reset_vel[1] = cur_vel/10;
			reset_vel[2] = cur_vel/10;
		}
		if (field == 1) {
		reset_vel[0] = cur_vel/5;
		reset_vel[1] = cur_vel/(-10);
		reset_vel[2] = cur_vel/(-10);
		}
	}
	
	//ls cal straight section
	//if ((tar_end == 1) && (raw_y > 1500) && (raw_y < (WALL_CAL - wall_dist)) && wall_dist) //encoder dist less than actual dist
	if ((tar_end == 1) && (raw_y < (WALL_CAL - wall_dist)) && wall_dist) //encoder dist less than actual dist
		off_y = raw_y - WALL_CAL + wall_dist; //negative
		
	//disable kI during blowing eco
	if (((tar_end >= 2) && (tar_end <= 4)) || (tar_end >= 6)) {
		ki = 0;
	} else {
		ki = KI;
	}
	
	//end path by switch
	if (gpio_read_input(&PE2)) {
		tar_end = tar_head;
		cur_x = tar_x;
		cur_y = tar_y;
		cur_deg = tar_deg*10;
	}
	
	//perpendicular PI
	err_pid = err * kp + err_sum * ki; // + (err-err_d) * 0.0;
	 
	//rotational P
	rotate *= RKP;
	
	angle *= 10;
	for (int i = 0; i < 3; i++) {
		vel[i] = int_sin(angle+i*1200)*maxvel/(-10000);	//calculate parallel distance
		if (dotcheck > 0.0)
			vel[i] -= int_sin(angle+i*1200+900)*err_pid/(-10000);	//subtract perpendicular negative feedback
		vel[i] *= vel_coeff;
		vel[i] -= rotate*acc;	//subtract rotational negative feedback
		vel[i] += (reset_rot + reset_vel[i]);
	}
		
	motor_set_vel(MOTOR1, vel[0], CLOSE_LOOP);
	motor_set_vel(MOTOR2, vel[1], CLOSE_LOOP);
	motor_set_vel(MOTOR3, vel[2], CLOSE_LOOP);  
	
	err_d = err;
	err_sum += err;
}

void auto_pole_climb(bool state){
	climbing_time = auto_get_ticks() - time;
	
	if (climbing_time < 500) { //release motor
		//Push forward at 50
		if (state) {
			motor_set_vel(MOTOR1, 0, CLOSE_LOOP);
			motor_set_vel(MOTOR2, 43, CLOSE_LOOP);
			motor_set_vel(MOTOR3, -48, CLOSE_LOOP);
			pneumatic_on(pole_clamp); //Clamp pole
		}
		motor_set_vel(MOTOR7, 0, OPEN_LOOP); //Lock biggold
		climb_dir = get_pos()->angle;
		climb_blow_pwm = 0;
		climb_speed = 1;
		brushless_servo_control(35);
	} else if (climbing_time < 1000) {
		if (state) {
			motor_set_vel(MOTOR1, 0, OPEN_LOOP);
			motor_set_vel(MOTOR2, 0, OPEN_LOOP);
			motor_set_vel(MOTOR3, 0, OPEN_LOOP);
			pneumatic_off(gripper_claw[field]); //claw
		}
	} else if (climbing_time < 1500) {
		pneumatic_off(gripper_push[field]); //collect
		brushless_control(50, true);
	} else if (climbing_time < 2000) {
		servo_control(gripper_servo[field], servo_up_val[field]);
	} else {
		if ((gpio_read_input(&PE3) || gpio_read_input(&PE9)) && !at_top) {
			at_top = true;
			top_time = auto_get_ticks();
		}
		
		if ((climbing_time > 2500) && !at_top) {
			pneumatic_on(gripper_push[field]); //push out
		}
		
		if ((climbing_time > 3800) && !at_top) {
			climb_speed = 0.8;
		}
		
		if (at_top) {
			motor_set_vel(MOTOR4, 0, OPEN_LOOP);
			motor_set_vel(MOTOR5, 0, OPEN_LOOP);
			motor_set_vel(MOTOR6, 0, OPEN_LOOP);
			brushless_control(0, true);
			if ((auto_get_ticks() - top_time) < 500) {
				pneumatic_on(gripper_push[field]); //placeholder
			} else if ((auto_get_ticks() - top_time) < 1800) {
				pneumatic_off(gripper_push[field]);
			} else if ((auto_get_ticks() - top_time) < 2000) {
				pneumatic_on(gripper_claw[field]);	
			} else {	
				servo_control(gripper_servo[field], servo_dn_val[field]);
			}
		} else {			
			motor_set_vel(MOTOR4, CLIMBING_SPEED*MOTOR4_FLIP*climb_speed, OPEN_LOOP);
			motor_set_vel(MOTOR5, CLIMBING_SPEED*MOTOR5_FLIP*climb_speed, OPEN_LOOP);
			motor_set_vel(MOTOR6, CLIMBING_SPEED*MOTOR6_FLIP*climb_speed, OPEN_LOOP);
			climb_blow_pwm = 20 + ((get_pos()->angle)<1800?0:(get_pos()->angle - 1800))*2;
			brushless_control(climb_blow_pwm, true);
		}
	}
	
	tft_clear();
	tft_prints(0,0,"[AUTO-CLIMB]");
	tft_prints(0,5,"POLE %3d",time/1000);
	tft_prints(0,6,"TIME %3d",auto_get_ticks()/1000);
	tft_prints(0,7,"ANGLE %3d",climb_blow_pwm);
	tft_prints(0,9,"%d", top_time);
	tft_update();
}

void auto_river_pause(void) {
	set_PID_FLAG(PID_OFF);
	brushless_control(0, true);
	
	if ((dist < THRESHOLD) && (Abs(degree_diff) < 2)) {
		auto_motor_stop();
	} else {
		auto_track_path(degree, degree_diff, cur_vel, false);
	}
	
	
	if (auto_get_ticks() - pause_time >= 7000) {
		set_PID_FLAG(PID_ON);
		brushless_time = auto_get_ticks();
		tar_end = 4;
		pid_state = RUNNING_MODE;
	}
	
	tft_clear();	
	tft_prints(0,0,"[PAUSED]");
	tft_prints(0,1,"TIME %3d",auto_get_ticks()/1000);
	tft_prints(0,2,"Start in %4d",(7000 - auto_get_ticks() + pause_time));
	tft_prints(0,4,"%d %d %d",cur_x, cur_y, cur_deg);
	tft_update();
}

/**
  * @brief  Locks hybrid in current location
  * @param  None
  * @retval None
  */
void auto_motor_stop(){
	vel[0] = 0;
	vel[1] = 0;
	vel[2] = 0;
	motor_set_vel(MOTOR1, vel[0], CLOSE_LOOP);
	motor_set_vel(MOTOR2, vel[1], CLOSE_LOOP);
	motor_set_vel(MOTOR3, vel[2], CLOSE_LOOP);
	motor_lock(MOTOR1);
	motor_lock(MOTOR2);
	motor_lock(MOTOR3);
}

void auto_robot_control(void) {
	if (arm_init) {
		arm_vel = (tar_arm - get_arm_pos())*ARM_SPEED/1200; //diff / range * speed
		arm_vel = ((arm_vel > -ARM_SPEED) ? ((arm_vel < ARM_SPEED) ? arm_vel : ARM_SPEED) : -ARM_SPEED);
		if (Abs(arm_vel) < 205) //biggold dead band >.<
			arm_vel += (205 * (arm_vel / Abs(arm_vel)));
		
		if ((tar_end == 1) && (!arrived || (arrived && ((auto_get_ticks() - arrived_time) < 1000)))) {
			tar_arm = 0;
		} else if ((Abs(cur_x) < 2000) && (retry_state == RETRY_HILL)) {
			tar_arm = 7500;
		} else if (Abs(cur_x) < 2000) {
			tar_arm = 3150;
		} else if (Abs(cur_x) < 4000) {
			tar_arm = 7500;
		} else {
			tar_arm = 12250;
		}
		
		motor_set_vel(MOTOR7, arm_vel*MOTOR7_FLIP, OPEN_LOOP);
	} else {
		lower_arm();
		if (gpio_read_input(&ARM_DN_LIMIT_PORT))
			arm_init = true;
	}
	
	if (tar_end <= 1) {
		brushless_servo_control(-85 + 85*2*field);
		brushless_control(0, true);
		if (!retry_state) {
			if (dist < 100) {
				brushless_control(45, true);
			}
		}
	} else if (tar_end <= 2) {
		if (!retry_state) {
			set_PID_FLAG(PID_ON);
			brushless_control_pid(900);
			brushless_servo_control(-90 + 90*2*field);
		}
	} else if (tar_end <= 3) {
		if (!retry_state) {
			brushless_servo_control(-85 + 85*2*field);
			if (auto_get_ticks() - brushless_time > 300)
				brushless_control_pid(770);
		}
	} else if (tar_end <= 4) {
		brushless_servo_control(-75 + 75*2*field);
		set_PID_FLAG(PID_ON);
		brushless_control_pid(720);
		if (auto_get_ticks() - brushless_time > 500)
			brushless_control_pid(680);	
		//if (auto_get_ticks() - brushless_time > 1200)				
			//brushless_servo_control(-65 + 65*2*field);
		if (auto_get_ticks() - brushless_time > 1500) {
			brushless_control_pid(900);
			//brushless_control(52, true);
			//brushless_servo_control(-75 + 75*2*field);
		}
	} else if (tar_end <= 5) {
		brushless_servo_control(-7 + 7*2*field);
		brushless_control_pid(700);
		if (auto_get_ticks() - brushless_time > 2000) {
			//brushless_servo_control(0);
			brushless_control_pid(780);
		}
		if (auto_get_ticks() - brushless_time > 3000)
			brushless_control_pid(890);
		if (auto_get_ticks() - brushless_time > 3500)
			brushless_control_pid(960);
		if (auto_get_ticks() - brushless_time > 4000)
			brushless_control_pid(1040);
		if (auto_get_ticks() - brushless_time > 4500)
			brushless_control_pid(1100);
	} else {
		set_PID_FLAG(PID_OFF);
		servo_control(SERVO3, 450);
	}
	
	/*
	if (((tar_end == 4) && arrived) || (tar_end == 5)) {
		ir_now = gpio_read_input(&PE6);
		
		if (!ir_now && ir_last) {
			tar_x = 6750;
			tar_y = 570;
			
			pause_time = auto_get_ticks();
			pid_state = PAUSE_MODE;
		}
	}
	ir_last = ir_now;
	*/
}

/**
  * @brief  Rotate 180 degrees for gyro calibration
  * @param  None
  * @retval None
  */
void auto_calibrate(){
	int cal_speed = -20;
	if (cur_deg <= 150)
		cal_speed = cur_deg/(-10)-5;
	if (cur_deg >= 1600)
		cal_speed = (1800-cur_deg)/(-10);
	
	if (cur_deg <= 1800) {
		motor_set_vel(MOTOR1, cal_speed, CLOSE_LOOP);
		motor_set_vel(MOTOR2, cal_speed, CLOSE_LOOP);
		motor_set_vel(MOTOR3, cal_speed, CLOSE_LOOP);
	} else {
		auto_motor_stop();
	}
	
	tft_clear();
	tft_prints(0,0,"X:%5d Y:%5d",cur_x,cur_y);
	tft_prints(0,1,"ANGLE %d",cur_deg);
	tft_update();
}

/**
  * @brief  Update menu display and executes menu control
  * @param  None
  * @retval None
  */
void auto_menu_update() {
	tft_clear();
	tft_prints(0,0,"[AUTO MODE]");
	tft_prints(0,6,"STATE %d%d%d%d", gpio_read_input(&PE10), gpio_read_input(&PE11), gpio_read_input(&PE0), gpio_read_input(&PE1));
	//tft_prints(0,7,"(Y)> STATE: %d", retry_state);
	tft_prints(0,7,"(Y)> %d", semi_auto_state);
	
	/*
	if (auto_get_flash(0,0) == PATH_ID) {
		tft_prints(0,1,"Path found!");
		tft_prints(0,2,"Length: %d",auto_get_flash(0,1));
		tft_prints(0,3,"[Press Start]");
	} else {
		tft_prints(0,1,"No path stored");	
	}
	*/
	tft_prints(0,1,"PRESS START");	
	
	if (rx_state == 0) {
		tft_prints(0,9,"Idle");
	} else {
		tft_prints(0,9,"Receiving");
	}
	tft_update();
	
	//if (button_pressed(BUTTON_XBC_START) && (auto_get_flash(0,0) == PATH_ID)){
	if (button_pressed(BUTTON_XBC_START)){
		if (!start_pressed) {
			start_pressed = true;
			
			if (retry_state == RETRY_POLE) {
				auto_ticks = get_full_ticks();
				at_top = false;
				gyro_pos_set(0, 0, 1800);
				pid_state = RETRY_MODE;
			} else {
				TARGET node_buffer;
				/*
				for (u8 i=0; i < auto_get_flash(0,1); i++) {
					if (auto_get_flash(1,i*NODE_SIZE) == 0)
						node_buffer.type = NODE_PASS;
					if (auto_get_flash(1,i*NODE_SIZE) == 1)
						node_buffer.type = NODE_STOP;
					node_buffer.x = auto_get_flash(1,i*NODE_SIZE+1);
					node_buffer.y = auto_get_flash(1,i*NODE_SIZE+2);
					node_buffer.deg = auto_get_flash(1,i*NODE_SIZE+3);
					node_buffer.curve = auto_get_flash(1,i*NODE_SIZE+4);	
					auto_tar_enqueue(node_buffer);			
				}
				*/
				
				#ifdef RED_FIELD
				//Red
				if (retry_state == RETRY_HILL) {
					node_buffer.type = NODE_PASS;
					node_buffer.x = 0;
					node_buffer.y = 1000;
					node_buffer.deg = 0;
					node_buffer.curve = 0;
					auto_tar_enqueue(node_buffer);
					node_buffer.type = NODE_STOP;
					node_buffer.x = 0;
					node_buffer.y = 2064;
					node_buffer.deg = 0;
					node_buffer.curve = 0;
					auto_tar_enqueue(node_buffer);
					node_buffer.type = NODE_STOP;
					node_buffer.x = -3315;
					node_buffer.y = 2064;
					node_buffer.deg = 315;
					node_buffer.curve = 0;
				} else {
					node_buffer.type = NODE_STOP;
					node_buffer.x = 0;
					node_buffer.y = 3250;
					node_buffer.deg = 0;
					node_buffer.curve = 0;
					auto_tar_enqueue(node_buffer);
					node_buffer.type = NODE_PASS;
					node_buffer.x = -875;
					node_buffer.y = 3250;
					node_buffer.deg = 0;
					node_buffer.curve = 0;
					auto_tar_enqueue(node_buffer);
					node_buffer.type = NODE_PASS;
					node_buffer.x = -3315;
					node_buffer.y = 2064;
					node_buffer.deg = 315;
					node_buffer.curve = -267;
				}
				
				auto_tar_enqueue(node_buffer);
				node_buffer.type = NODE_STOP;
				node_buffer.x = -6750;
				node_buffer.y = 570;
				node_buffer.deg = 0;
				node_buffer.curve = 192;
				auto_tar_enqueue(node_buffer);
				node_buffer.type = NODE_STOP;
				node_buffer.x = -7460;
				node_buffer.y = 350;
				node_buffer.deg = 0;
				node_buffer.curve = 0;
				auto_tar_enqueue(node_buffer);
				node_buffer.type = NODE_PASS;
				node_buffer.x = -9831;
				node_buffer.y = 690;
				node_buffer.deg = 90;
				node_buffer.curve = 0;
				auto_tar_enqueue(node_buffer);
				node_buffer.type = NODE_PASS;
				node_buffer.x = -12409;
				node_buffer.y = 2570;
				node_buffer.deg = -175;
				node_buffer.curve = 281;
				auto_tar_enqueue(node_buffer);
				node_buffer.type = NODE_PASS;
				node_buffer.x = -12900;
				node_buffer.y = 4075;
				node_buffer.deg = 180;
				node_buffer.curve = 281;
				auto_tar_enqueue(node_buffer);
				node_buffer.type = NODE_PASS;
				node_buffer.x = -12900;
				node_buffer.y = 5000;
				node_buffer.deg = 180;
				node_buffer.curve = 0;
				auto_tar_enqueue(node_buffer);
				node_buffer.type = NODE_STOP;
				node_buffer.x = -12900;
				node_buffer.y = 6500;
				node_buffer.deg = 180;
				node_buffer.curve = 0;
				auto_tar_enqueue(node_buffer);
				#else
				//Blue
				if (retry_state == RETRY_HILL) {
					node_buffer.type = NODE_PASS;
					node_buffer.x = 0;
					node_buffer.y = 1000;
					node_buffer.deg = 0;
					node_buffer.curve = 0;
					auto_tar_enqueue(node_buffer);
					node_buffer.type = NODE_STOP;
					node_buffer.x = 0;
					node_buffer.y = 2064;
					node_buffer.deg = 0;
					node_buffer.curve = 0;
					auto_tar_enqueue(node_buffer);
					node_buffer.type = NODE_STOP;
					node_buffer.x = 3315;
					node_buffer.y = 2064;
					node_buffer.deg = 45;
					node_buffer.curve = 0;
				} else {
					node_buffer.type = NODE_STOP;
					node_buffer.x = 0;
					node_buffer.y = 3260;
					node_buffer.deg = 0;
					node_buffer.curve = 0;
					auto_tar_enqueue(node_buffer);
					node_buffer.type = NODE_PASS;
					node_buffer.x = 875;
					node_buffer.y = 3260;
					node_buffer.deg = 0;
					node_buffer.curve = 0;
					auto_tar_enqueue(node_buffer);
					node_buffer.type = NODE_PASS;
					node_buffer.x = 3315;
					node_buffer.y = 2064;
					node_buffer.deg = 45;
					node_buffer.curve = 267;
				}
				
				auto_tar_enqueue(node_buffer);
				node_buffer.type = NODE_STOP;
				node_buffer.x = 6750;
				node_buffer.y = 570;
				node_buffer.deg = 0;
				node_buffer.curve = -192;
				auto_tar_enqueue(node_buffer);
				node_buffer.type = NODE_STOP;
				node_buffer.x = 7460;
				node_buffer.y = 350;
				node_buffer.deg = 0;
				node_buffer.curve = 0;
				auto_tar_enqueue(node_buffer);
				
				if (semi_auto_state) {
					node_buffer.type = NODE_PASS;
					node_buffer.x = 9831;
					node_buffer.y = 690;
					node_buffer.deg = -90;
					node_buffer.curve = 0;
					auto_tar_enqueue(node_buffer);
					node_buffer.type = NODE_PASS;
					node_buffer.x = 12409;
					node_buffer.y = 2570;
					node_buffer.deg = 175;
					node_buffer.curve = -281;
					auto_tar_enqueue(node_buffer);
					node_buffer.type = NODE_PASS;
					node_buffer.x = 12900;
					node_buffer.y = 4075;
					node_buffer.deg = 180;
					node_buffer.curve = -281;
					auto_tar_enqueue(node_buffer);
					node_buffer.type = NODE_STOP;
					node_buffer.x = 12900;
					node_buffer.y = 5000;
					node_buffer.deg = 180;
					node_buffer.curve = 0;
					auto_tar_enqueue(node_buffer);
					
					tar_end = 5;
				}
				
				/*
				node_buffer.type = NODE_STOP;
				node_buffer.x = 12900;
				node_buffer.y = 6500;
				node_buffer.deg = 180;
				node_buffer.curve = 0;
				auto_tar_enqueue(node_buffer);
				*/
				
				#endif

				auto_reset();
				
				pid_state = RUNNING_MODE;
			}
		}
	} else {
		start_pressed = false;
	}
	
	if (button_pressed(BUTTON_XBC_Y)){
		if (!y_pressed) {
			y_pressed = true;
			
			/*
			retry_state++;
			retry_state %= RETRY_STATES_NO;
			*/
			
			semi_auto_state++;
			semi_auto_state %= 2;
		}
	} else {
		y_pressed = false;
	}
	
	if (to_be_saved) {
		FLASH_Unlock();
		FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
		
		FLASH_ErasePage(HEADER_BASE_ADDR);
		FLASH_ProgramHalfWord(HEADER_BASE_ADDR, PATH_ID);
		FLASH_ProgramHalfWord(HEADER_BASE_ADDR+2, (uint16_t)rx_path_length);
		
		FLASH_ErasePage(PATH_BASE_ADDR);
		for (u8 p=0; p<rx_path_length; p++) {
			for (u8 q=0; q<NODE_SIZE; q++) {
				FLASH_ProgramWord(PATH_BASE_ADDR+(p*NODE_SIZE+q)*4, rx_node_list[p][q]);
			}
		}
		
		FLASH_Lock();		
		to_be_saved = false;
	}
}

/**
  * @brief  Update PID variables
  * @param  None
  * @retval None
  */
void auto_var_update() {
	passed = auto_get_ticks() - start;
	
	u32 reading1, reading2;
	
	#ifndef DEBUG_MODE
		reading1 = get_ls_cal_reading(0);
		reading2 = get_ls_cal_reading(2);
		if (reading1 == 0)
			reading1 = 200;
		if (reading2 == 0)
			reading2 = 200;
		if (Abs(reading2 - reading1) < LS_DIFF) {
			wall_dist = (reading1 + reading2)/2;
			//if (!((tar_end == 4) && (dist < 200)) && !(tar_end >= 5)) { //stop shift when approach hill3
			
			if (((tar_end >= 2) && (tar_end <= 3) && retry_state == 0) || ((tar_end == 4) && (dist > 200))) { //stop shift when approach hill3
				if (field == 0) {
					if (wall_dist < INNER_DIST)
						transform[1][0] -= (SHIFT/7000.0);
					if ((wall_dist > OUTER_DIST) && (wall_dist < 1000))
						transform[1][0] += (SHIFT/7000.0);
				}
				if (field == 1) {
					if (wall_dist < INNER_DIST)
						transform[1][0] += (SHIFT/7000.0);
					if ((wall_dist > OUTER_DIST) && (wall_dist < 1000))
						transform[1][0] -= (SHIFT/7000.0);
				}
			}
		} else {
			wall_dist = 0;
		}
	#endif
	
	raw_x = transform[0][0]*get_X() + transform[0][1]*get_Y();
	raw_y = transform[1][0]*get_X() + transform[1][1]*get_Y();
	
	cur_x = raw_x - off_x;
	cur_y = raw_y - off_y;
	cur_deg = (get_angle() - off_deg) % 3600;
	
	if (tar_queue[tar_end-1].curve == 0) {
		degree = tar_dir;
	} else if (tar_queue[tar_end-1].curve < 0) {
		degree = 180 - int_arc_tan2(tar_cen_y - cur_y, tar_cen_x - cur_x);
	} else if (tar_queue[tar_end-1].curve > 0) {
		degree = 0 - int_arc_tan2(tar_cen_y - cur_y, tar_cen_x - cur_x);
	}
	degree -= (cur_deg/10);
	
	degree_diff = tar_deg - (int)(cur_deg/10);
	degree_diff = (degree_diff+1080)%360;
	if (degree_diff > 180)
		degree_diff -= 360;
	
	dist = pythag(cur_x, cur_y, tar_x, tar_y);
	time = auto_get_ticks();
	//measured_vel = (int)((double)((dist - dist_last)*1000)/((double)(time - time_last)));
	
	//set target degree
	int diff = auto_tar_ret(tar_end-1).deg - auto_tar_ret(tar_end-2).deg;
	if (diff) {
		if (diff > 180)
			diff -= 360;
		if (diff < -180)
			diff += 360;		
		deg_ratio = (pythag(ori_x, ori_y, tar_x, tar_y) - dist)*diff;
		deg_ratio /= pythag(ori_x, ori_y, tar_x, tar_y);
		tar_deg = (int)((double)auto_tar_ret(tar_end-2).deg + deg_ratio);
	}
	
	dist_last = dist;
	time_last = time;
}

/**
  * @brief  Update following actions for driving motors and display data
  * @param  None
  * @retval None
  */
void auto_motor_update(){
	s32 temp_deg;
	
	if ((dist < THRESHOLD) && (Abs(degree_diff) < 2)) {
		if (!arrived) {
			arrived = true;
			arrived_time = auto_get_ticks();
		}
		
		//ensure touches both switches before next path
		u8 side_switch_states = 0;
		if (((tar_x == 0) || (tar_x == -12900)) && (field == 0))
			side_switch_states = gpio_read_input(&PE11) & gpio_read_input(&PE10);
		if (((tar_x == 0) || (tar_x == 12900)) && (field == 1))
			side_switch_states = gpio_read_input(&PE1) & gpio_read_input(&PE0);
		
		if (auto_tar_queue_len()) {
			if ((tar_x == 0) || (tar_x == 12900)) {
				if (side_switch_states && !arrived) {
					arrived = true;
					arrived_time = auto_get_ticks();
				}
			} else {
				if (!arrived) {
					arrived = true;
					arrived_time = auto_get_ticks();
				}
			}
			
			if (arrived) {
				if (tar_end == 1) {
					if ((auto_get_ticks() - arrived_time) > 1500)
						auto_tar_dequeue();
					else
						auto_motor_stop();
				} else if (tar_end == 2) {
					if ((auto_get_ticks() - arrived_time) > 100)
						auto_tar_dequeue();
					else
						auto_motor_stop();
				} else {
					auto_tar_dequeue();
				}
			}
			
		} else {
			pid_stopped = true;
			auto_motor_stop();
			pid_state = MENU_MODE;
		}
	} else if (gpio_read_input(&PE2)) {
		auto_motor_stop();
		pid_state = CLIMBING_MODE;
	} else if (!((tar_end == 5) && ((auto_get_ticks() - start) < 4000))){ //check midway stop
		auto_track_path(degree, degree_diff, cur_vel, false);
	} else {
		auto_motor_stop(); //stop to ecoblow
	}
	auto_robot_control();
	
	//print debug info
	tft_clear();
	tft_prints(0,0,"[AUTO MODE]");
	
	tft_prints(0,1,"X %5d -> %5d",cur_x,tar_x);
	tft_prints(0,2,"Y %5d -> %5d",cur_y,tar_y);
	tft_prints(0,3,"D %5d -> %5d",cur_deg,tar_deg);
	
	
	/*
	tft_prints(0,1,"X %5d",cur_x);
	tft_prints(0,2,"Y %5d",cur_y);
	tft_prints(0,3,"D %5d",cur_deg);
	*/
	tft_prints(0,4,">> %2d / %2d",tar_end,tar_head);
	tft_prints(0,5,"VEL %3d %3d %3d",vel[0],vel[1],vel[2]);
	tft_prints(0,6,"TIM %3d",time/1000);
	/*
	//tft_prints(0,7,"Test %d",measured_vel);
	//tft_prints(0,7,"Test %d %d %d", arm_vel, get_arm_pos(), tar_arm);
	//tft_prints(0,7,"Test %d %d", dist, degree_diff);
	//tft_prints(0,7,"Test %d", err_sum);
	//tft_prints(0,7,"Test %d %d", get_X(), get_Y());
	tft_prints(0,7,"Test %d %d", side_switch_val, back_switch_val);
	tft_prints(0,7,"Test %d", (auto_get_ticks() - arrived_time));
	*/
	
	//tft_prints(0,8,"Trans: %d", (int)(transform[1][0]*700));
	//tft_prints(0,9,"W %d %d %d", get_ls_cal_reading(0), get_ls_cal_reading(2), wall_dist);
	tft_prints(0,9,(get_PID_FLAG()?"PID ON":"PID OFF"));
	tft_update();
	
	temp_deg = (cur_deg < -1800) ? (cur_deg+3600) : ((cur_deg >= 1800) ? (cur_deg-3600) : cur_deg);
	//uart_tx(COM2, (uint8_t *)"%d, %d, %d, %d, %d, %d, %d, %d\n", time, cur_x, cur_y, temp_deg, side_switch_val, back_switch_val, dist, err_sum);
	//uart_tx(COM2, (uint8_t *)"%d, %d, %d, %d, %d\n", time, cur_x, cur_y, temp_deg, dist);
	
	//handle input
	/*
	if (button_pressed(BUTTON_XBC_BACK)) {
		if (!back_pressed) {
			back_pressed = true;
			auto_motor_stop();
			auto_init();
			pid_state = MENU_MODE;
		}
	} else {
		back_pressed = false;
	}
	*/
}

void USART3_IRQHandler(void) {
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){
		const uint8_t byte = USART_ReceiveData(USART3);
		if (rx_state == 0) { //idle
			if (byte == 147)
				rx_state = 1;
		} else if (rx_state == 1) { //receive length
			rx_path_length = byte;
			rx_count = 0;
			rx_pointer = 0;
			rx_node_no = 0;
			tar_head = 0; //reset target queue
			rx_state = 2;
		} else if (rx_state == 2) { //receive path
			rx_buffer[rx_count] = byte;
			rx_count++;
			if (rx_count == 4) { //finish receive one value
				rx_count = 0;
				rx_node_list[rx_node_no][rx_pointer] = rx_merge();
				rx_pointer++;
			}
			if (rx_pointer == NODE_SIZE) { //finish receive whole node
				rx_pointer = 0;
				rx_node_no++;
			}
			if (rx_node_no == rx_path_length){ //finish receive all nodes
				to_be_saved = true;
				rx_state = 0;
			}
		}
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}

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
  * @version V1.0.0
  * @date    11 Feb, 2016
  * @brief   Library for hybrid auto movement mode, in particular for PID
	*          movement tracking.
  ******************************************************************************
  */

#include "auto_mode.h"

#define THRESHOLD 10
#define CONST_VEL 50

struct target {
	int x;
	int y;
	int deg;
	double curve; //curvature scaled up by 1000
	bool stop;
};

//mode variables
bool is_running;
bool up_pressed, dn_pressed, start_pressed;
int path_id;

//path target queue
struct target tar_queue[50];
int tar_head, tar_end;

//path target variables
int tar_x, tar_y, tar_deg, tar_dir;
int tar_rad, tar_cen_x, tar_cen_y;
int ori_x, ori_y;

//auto properties
int cur_x, cur_y, cur_deg;
int cor_x, cor_y;
int vel[3];
int degree, degree_diff, dist, speed;
int start, passed;
int err_d;
int auto_ticks = 0;

/**
  * @brief  Add target to queue
  * @param  x: x-coordinate of target
  * @param  y: y-coordinate of target
  * @param  deg: orientation of hybrid at target
  * @param  curve: curvature of path, divided by 1000
  * @param  stop: deccelerate and pause at target if true
  * @retval None
  */
void auto_tar_enqueue(int x, int y, int deg, int curve, bool stop) {
	tar_queue[tar_head].x = x;
	tar_queue[tar_head].y = y;
	tar_queue[tar_head].deg = deg;
	tar_queue[tar_head].curve = curve;
	tar_queue[tar_head].stop = stop;
	tar_head++;
}

/**
  * @brief  Execute topmost target in queue
  * @param  None
  * @retval None
  */
void auto_tar_dequeue() {
	int mid_length;
	if (tar_end && tar_queue[tar_end-1].stop)
		start = auto_get_ticks();
	if (tar_end) {
		ori_x = tar_queue[tar_end-1].x;
		ori_y = tar_queue[tar_end-1].y;
	} else {
		ori_x = 0;
		ori_y = 0;
	}
	tar_x = tar_queue[tar_end].x;
	tar_y = tar_queue[tar_end].y;
	tar_deg = tar_queue[tar_end].deg;
	tar_dir = 90 - int_arc_tan2(tar_y - ori_y, tar_x - ori_x);
	
	tar_rad = 0;
	if (tar_queue[tar_end].curve)
		tar_rad = 1000/tar_queue[tar_end].curve;
	
	mid_length = Sqr(tar_x - ori_x) + Sqr(tar_y - ori_y);
	mid_length = Sqrt(Sqr(tar_rad) - mid_length/4);
	mid_length *= (tar_rad / Abs(tar_rad));
	tar_cen_x = (ori_x + tar_x)/2 + mid_length * int_cos(tar_dir * 10) / 10000;
	tar_cen_y = (ori_y + tar_y)/2 - mid_length * int_sin(tar_dir * 10) / 10000;
	
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
	is_running = false;
	up_pressed = false;
	dn_pressed = false;
	start_pressed = false;
	path_id = 0;
}

/**
  * @brief  Reset running variables before starting PID
  * @param  None
  * @retval None
  */
void auto_reset() {
	tar_head = 0;
	tar_end = 0;
	err_d = 0;
	dist = 0;
	degree_diff = 0;
	tar_x = 0;
	tar_y = 0;
	auto_ticks = get_full_ticks();
	start = 0;
	gyro_pos_set(0,0,0);
}

/**
  * @brief  Return state of hybrid auto mode
  * @param  None
  * @retval True if hybrid is in PID mode
  */
bool auto_get_state() {
	return is_running;
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
	
	//determine if overflow
	dotcheck = (cur_x-tar_x)*(ori_x-tar_x) + (cur_y-tar_y)*(ori_y-tar_y);
	dotcheck /= Sqrt(Sqr(ori_x-tar_x)+Sqr(ori_y-tar_y));
	dotcheck /= Sqrt(Sqr(cur_x-tar_x)+Sqr(cur_y-tar_y));
	if (dotcheck <= 0.0) {
		if (auto_tar_queue_len() && !tar_queue[tar_end-1].stop)
			auto_tar_dequeue();
		else
			angle = 90 - int_arc_tan2(tar_y - cur_y, tar_x - cur_x) - (int)(cur_deg/10);
	}
	
	//determine velocity coefficient
	double acc = passed / 2000.0;
	double dec = dist / 400.0;
	//double dec = sqrt(dist / 680.0);
	if (acc > 1.0)
		acc = 1.0;
	if (!tar_queue[tar_end-1].stop)
		dec = 1.0;
	double vel_coeff = acc < dec ? acc : dec;
	if (vel_coeff > 1.0)
		vel_coeff = 1.0;
	
	//find perpendicular dist
	if (tar_queue[tar_end-1].curve == 0.0) { //straight line
		p = tar_x - ori_x;
		q = tar_y - ori_y;
		err = p*(tar_y - cur_y) - q*(tar_x - cur_x);
		err /= (int)(Sqrt(Sqr(p)+Sqr(q)));
	} else { //curve
		err = Sqrt(Sqr(tar_cen_x - cur_x) + Sqr(tar_cen_y - cur_y));
		err *= (tar_rad / Abs(tar_rad));
		err = tar_rad - err;
	}
	
	//perpendicular PD
	err_pid = err * 0.2 + (err-err_d) * 0.0;
	
	//rotational P
	rotate *= 0.5;
	
	angle *= 10;
	for (int i = 0; i < 3; i++) {
		vel[i] = int_sin(angle+i*1200)*maxvel/(-10000);	//calculate parallel distance
		if (dotcheck > 0.0)
			vel[i] -= int_sin(angle+i*1200+900)*err_pid/(-10000);	//subtract perpendicular negative feedback
		vel[i] *= vel_coeff;
		vel[i] -= rotate*acc;	//subtract rotational negative feedback
	}
	
	motor_set_vel(MOTOR1, vel[0], CLOSE_LOOP);
	motor_set_vel(MOTOR2, vel[1], CLOSE_LOOP);
	motor_set_vel(MOTOR3, vel[2], CLOSE_LOOP);
	
	err_d = err;
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
	tft_prints(2,1,"Straight");
	tft_prints(2,2,"Circle");
	tft_prints(2,3,"8-Figure");
	tft_prints(0,path_id+1,">");	
	tft_update();
	
	if (button_pressed(BUTTON_XBC_START)){
		if (!start_pressed) {
			start_pressed = true;
			auto_reset();
			is_running = true;
			switch (path_id) {
				case 0:
					auto_tar_enqueue(0, 1500, 0, 0.0, true);
					break;
				case 1:
					auto_tar_enqueue(1000, 1000, 0, 1.0, false);
					auto_tar_enqueue(2000, 0, 0, 1.0, false);
					auto_tar_enqueue(1000, -1000, 0, 1.0, false);
					auto_tar_enqueue(0, 0, 0, 1.0, true);
					break;
				case 2:
					auto_tar_enqueue(500, 0, 0, 0.0, false);
					auto_tar_enqueue(1000, 500, 0, -2.0, false);
					auto_tar_enqueue(500, 1000, 0, -2.0, false);
					auto_tar_enqueue(0, 500, 0, -2.0, false);
					auto_tar_enqueue(0, -500, 0, 0.0, false);
					auto_tar_enqueue(-500, -1000, 0, 2.0, false);
					auto_tar_enqueue(-1000, -500, 0, 2.0, false);
					auto_tar_enqueue(-500, 0, 0, 2.0, false); 
					auto_tar_enqueue(0, 0, 0, 0.0, true);
					break;
			}
		}
	} else {
		start_pressed = false;
	}
	
	if (button_pressed(BUTTON_XBC_N) && (path_id > 0)) {
		if (!up_pressed) {
			up_pressed = true;
			path_id--;
		}
	} else {
		up_pressed = false;
	}
	
	if (button_pressed(BUTTON_XBC_S) && (path_id < 2)) {
		if (!dn_pressed) {
			dn_pressed = true;
			path_id++;
		}
	} else {
		dn_pressed = false;
	}
}

/**
  * @brief  Update PID variables
  * @param  None
  * @retval None
  */
void auto_var_update() {
	passed = auto_get_ticks() - start;
	cur_deg = get_angle();
	cor_x = 0;
	cor_y = 0;
	xy_rotate(&cor_x, &cor_y, (-1)*cur_deg);
	cur_x = get_pos()->x + cor_x - 0;
	cur_y = get_pos()->y + cor_y - 0;
	
	degree = tar_dir;
	if (tar_queue[tar_end-1].curve < 0)
		degree = 90 - int_arc_tan2(tar_cen_y - cur_y, tar_cen_x - cur_x) + 90;
	if (tar_queue[tar_end-1].curve > 0)
		degree = 90 - int_arc_tan2(tar_cen_y - cur_y, tar_cen_x - cur_x) - 90;
	degree -= (cur_deg/10);
	
	degree_diff = tar_deg - (int)(cur_deg/10);
	degree_diff = (degree_diff+1080)%360;
	if (degree_diff > 180)
		degree_diff -= 360;
	
	dist = Sqrt(Sqr(tar_x - cur_x) + Sqr(tar_y - cur_y));		
}

/**
  * @brief  Update following actions for driving motors and display data
  * @param  None
  * @retval None
  */
void auto_motor_update(){
	if ((dist < THRESHOLD) && (Abs(degree_diff) < 2)) {
		if (auto_tar_queue_len())
			auto_tar_dequeue();
		else
			auto_motor_stop();
	} else {
		auto_track_path(degree, degree_diff, CONST_VEL, false);
	}
	
	//print debug info
	tft_clear();
	tft_prints(0,0,"X:%5d Y:%5d",cur_x,cur_y);
	tft_prints(0,1,"ANGLE %d",cur_deg);
	tft_prints(0,2,"TAR %d %d",tar_x,tar_y);
	tft_prints(0,3,"VEL %3d %3d %3d",vel[0],vel[1],vel[2]);
	tft_prints(0,4,"TIM %3d",auto_get_ticks()/1000);
	tft_update();
}

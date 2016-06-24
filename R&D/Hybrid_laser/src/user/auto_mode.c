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
#define CONST_VEL 50

//#define DEBUG_MODE

//Ground: 0 = Red, 1 = Blue
u8 field = 1;

//Blue Field transformation
float transform[2][2] = {{1, 0}, {0.057143, 1}};

//mode variables
PID_MODE pid_state;
bool pid_stopped;
bool start_pressed, back_pressed;
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
int start, passed;
int err_d;
int auto_ticks = 0;

u8 side_switch_val = 0;
u8 back_switch_val = 0;

//UART receiver
u8 rx_count = 0;
u8 rx_pointer = 0;
TARGET rx_node;
uint8_t rx_buffer[4] = {0,0,0,0};
bool is_loaded = false;


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
	
	if (tar_end) {
		ori_x = tar_queue[tar_end-1].x;
		ori_y = tar_queue[tar_end-1].y;
	} else {
		ori_x = 0;
		ori_y = 0;
	}
	
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
	
	tar_head = 0;
	tar_end = 0;
	is_loaded = false;
}

/**
  * @brief  Reset running variables before starting PID
  * @param  None
  * @retval None
  */
void auto_reset() {
	//reset variables
	err_d = 0;
	dist = 0;
	degree_diff = 0;
	tar_x = 0;
	tar_y = 0;
	tar_deg = 0;
	off_x = 0;
	off_y = 0;
	off_deg = 0;
	deg_ratio = 0;
	start = 0;
	pid_stopped = false;
	
	//reset local timer
	auto_ticks = get_full_ticks();
	
	//reset gyro location
	gyro_pos_set(0,0,0);
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
		if (auto_tar_queue_len() && (tar_queue[tar_end-1].type == NODE_PASS))
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
	if (tar_queue[tar_end-1].type == NODE_PASS)
		dec = 1.0;
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
	
	back_switch_val = (cur_y <= 0)*4 + gpio_read_input(&PE6)*2 + gpio_read_input(&PE7);
	if (field == 0)
		side_switch_val = (cur_x >= 0)*4 + gpio_read_input(&PE11)*2 + gpio_read_input(&PE10);
	if (field == 1)
		side_switch_val = (cur_x <= 0)*4 + gpio_read_input(&PE8)*2 + gpio_read_input(&PE9);
	
	if ((side_switch_val == 3) || (side_switch_val & 4)) {
		off_x = raw_x;
		if ((side_switch_val == 3) || (side_switch_val == 7))
			off_deg = get_angle();
	}	
	if ((back_switch_val == 3) || (back_switch_val & 4)) {
		off_y = raw_y;
		if ((back_switch_val == 3) || (back_switch_val == 7))
			off_deg = get_angle();
	}
	
	if ((side_switch_val & 2) && !(side_switch_val & 1)) {
		reset_rot = 2;
	}
	if (!(side_switch_val & 2) && (side_switch_val & 1)) {
		reset_rot = -2;		
	}
	if ((back_switch_val & 2) && !(back_switch_val & 1)) {
		reset_rot = 2;
	}
	if (!(back_switch_val & 2) && (back_switch_val & 1)) {
		reset_rot = -2;		
	}
	
	if (side_switch_val == 4) {
		if (field == 0) {
			reset_vel[0] = -10;
			reset_vel[1] = 5;
			reset_vel[2] = 5;
		}
		if (field == 1) {
		reset_vel[0] = 10;
		reset_vel[1] = -5;
		reset_vel[2] = -5;
		}
	}
	if (back_switch_val == 4) {
		reset_vel[0] = 0;
		reset_vel[1] = -10;
		reset_vel[2] = 10;
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
		vel[i] += (reset_rot + reset_vel[i]);
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
	tft_prints(0,2,"State: %d %d",rx_count, rx_pointer);
	if (is_loaded) {
		tft_prints(0,5,"Press Start!");
		tft_prints(0,6,"Length: %d",tar_head);
	} else {
		tft_prints(0,5,"Load path now");
	}
	tft_update();
	
	if (button_pressed(BUTTON_XBC_START) && is_loaded){
		if (!start_pressed) {
			start_pressed = true;
			auto_reset();
			pid_state = RUNNING_MODE;
		}
	} else {
		start_pressed = false;
	}
}

/**
  * @brief  Update PID variables
  * @param  None
  * @retval None
  */
void auto_var_update() {
	passed = auto_get_ticks() - start;
	
	#ifdef DEBUG_MODE
		raw_x = get_pos()->x;
		raw_y = get_pos()->y;
	#endif
	
	#ifndef DEBUG_MODE
		raw_x = transform[0][0]*(get_pos()->x) + transform[0][1]*(get_pos()->y);
		raw_y = transform[1][0]*(get_pos()->x) + transform[1][1]*(get_pos()->y);
	#endif
	
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
	
	//set target degree
	int diff = auto_tar_ret(tar_end-1).deg - auto_tar_ret(tar_end-2).deg;
	if (diff) {
		if (diff > 180)
			diff -= 360;
		if (diff < -180)
			diff += 360;		
		deg_ratio = (pythag(ori_x, ori_y, tar_x, tar_y) - dist)*diff;
		deg_ratio /= pythag(ori_x, ori_y, tar_x, tar_y);
		tar_deg = auto_tar_ret(tar_end-2).deg + (int)deg_ratio;
	}
}

/**
  * @brief  Update following actions for driving motors and display data
  * @param  None
  * @retval None
  */
void auto_motor_update(){
	if ((dist < THRESHOLD) && (Abs(degree_diff) < 2)) {
		if (auto_tar_queue_len()) {
			auto_tar_dequeue();
		} else {
			pid_stopped = true;
			auto_motor_stop();
		}
	} else {
		auto_track_path(degree, degree_diff, CONST_VEL, false);
	}
	
	//print debug info
	tft_clear();
	tft_prints(0,0,"[AUTO MODE]");
	tft_prints(0,1,"X %5d -> %5d",cur_x,tar_x);
	tft_prints(0,2,"Y %5d -> %5d",cur_y,tar_y);
	tft_prints(0,3,"D %5d -> %5d",cur_deg,tar_deg);
	tft_prints(0,4,">> %2d / %2d",tar_end,tar_head);
	tft_prints(0,5,"VEL %3d %3d %3d",vel[0],vel[1],vel[2]);
	tft_prints(0,6,"TIM %3d",auto_get_ticks()/1000);
	tft_prints(0,8,"%d %d",get_pos()->x,get_pos()->y);
	tft_prints(0,9,"%d %d B:%d",gpio_read_input(&PE6),gpio_read_input(&PE7),back_switch_val);
	tft_update();
	
	//handle input
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
}

void USART2_IRQHandler(void) {
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){
		const uint8_t byte = USART_ReceiveData(USART2);
		rx_buffer[rx_count] = byte;
		rx_count++;
		if (rx_count == 4) {
			rx_count = 0;
			switch(rx_pointer) {
				case 0:
					if (rx_merge())
						rx_node.type = NODE_STOP;
					else
						rx_node.type = NODE_PASS;
					break;
				case 1:
					rx_node.x = rx_merge();
					break;
				case 2:
					rx_node.y = rx_merge();
					break;
				case 3:
					rx_node.deg = rx_merge();
					break;
				case 4:
					rx_node.curve = rx_merge();
					break;
			}
			rx_pointer++;
		}
		if (rx_pointer == 5) {
			rx_pointer = 0;
			auto_tar_enqueue(rx_node);
			is_loaded = true;
		}
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}

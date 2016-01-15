#include "main.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

#define THRESHOLD 50
#define CONST_VEL 50

struct target {
	int x;
	int y;
	int deg;
	bool stop;
};

struct target tar_queue[50];
int tar_head, tar_end;

int tar_x, tar_y, tar_deg, tar_dir;
int cur_x, cur_y, cur_deg;
int cor_x, cor_y;
int ori_x, ori_y;
int vel[3];
int degree, degree_diff, dist, speed;
int start, passed;
int err_d, temp;

char err_msg[100];

void tar_enqueue(int x, int y, int deg, bool stop) {
	tar_queue[tar_head].x = x;
	tar_queue[tar_head].y = y;
	tar_queue[tar_head].deg = deg;
	tar_queue[tar_head].stop = stop;
	tar_head++;
}

void tar_dequeue() {
	if (tar_end && tar_queue[tar_end-1].stop)
		start = get_ticks();
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
	tar_end++;
}

int tar_queue_length() {
	return tar_head - tar_end;
}

void can_track_path(int angle, int rotate, int maxvel)
{
	int p, q;
	int err, err_pid;
	double dotcheck;
	
	//determine if overflow
	dotcheck = (cur_x-tar_x)*(ori_x-tar_x) + (cur_y-tar_y)*(ori_y-tar_y);
	dotcheck /= Sqrt(Sqr(ori_x-tar_x)+Sqr(ori_y-tar_y));
	dotcheck /= Sqrt(Sqr(cur_x-tar_x)+Sqr(cur_y-tar_y));
	if (dotcheck <= 0.0) {
		if (tar_queue_length() && !tar_queue[tar_end-1].stop)
			tar_dequeue();
		else
			angle = 90 - int_arc_tan2(tar_y - cur_y, tar_x - cur_x) - (int)(cur_deg/10);
	}
	
	//determine velocity coefficient
	double acc = passed / 2000.0;
	double dec = dist / 600.0;
	if (acc > 1.0)
		acc = 1.0;
	if (!tar_queue[tar_end-1].stop)
		dec = 1.0;
	double vel_coeff = acc < dec ? acc : dec;
	if (vel_coeff > 1.0)
		vel_coeff = 1.0;
	
	//find perpendicular dist
	p = tar_x - ori_x;
	q = tar_y - ori_y;
	err = p*(tar_y - cur_y) - q*(tar_x - cur_x);
	err /= Sqrt(Sqr(p)+Sqr(q));
	
	//perpendicular PD
	temp = err-err_d;
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

void can_motor_stop(){
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

void can_motor_update(){
	if ((dist < THRESHOLD) && (Abs(degree_diff) < 5)) {
		if (tar_queue_length())
			tar_dequeue();
		else
			can_motor_stop();
	} else {
		can_track_path(degree, degree_diff, CONST_VEL);
	}
	
	//print debug info
	tft_clear();
	tft_prints(0,0,"X:%5d Y:%5d",cur_x,cur_y);
	tft_prints(0,1,"ANGLE %d",cur_deg);
	tft_prints(0,2,"%d",err_d);
	tft_prints(0,3,"%d",temp);
	//tft_prints(0,3,"X:%5d Y:%5d",tar_x,tar_y);
	//tft_prints(0,4,"%5dmm  %3dd",dist,degree);
	//tft_prints(0,5,"ROTATE %3dd",degree_diff);
	tft_prints(0,7,"VEL %3d %3d %3d",vel[0],vel[1],vel[2]);
	//tft_prints(0,8,"MAX %3d",speed);
	tft_prints(0,9,"TIM %3d",get_seconds());
	tft_update();
}

void can_calibrate(){
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
		can_motor_stop();
	}
	
	tft_clear();
	tft_prints(0,0,"X:%5d Y:%5d",cur_x,cur_y);
	tft_prints(0,1,"ANGLE %d",cur_deg);
	tft_update();
}
	
/*
void move_line(int x, int y, int deg, int segment) {
	double ratio;
	for (int i = 0; i < segment; i++) {
		ratio = (i+1)/(double)segment;
		tar_enqueue((x - cur_x)*ratio, (y - cur_y)*ratio, (deg - cur_deg/10)*ratio);
	}
}

//A = point that curve passes through
//B = destination point
//M = midpt between origin and B
//N = pt extended from M through A to generate curve
void move_bezier(int ax, int ay, int bx, int by, int segment) {
	int mx, my, nx, ny;
	int px, py, qx, qy;
	double ratio;
	
	mx = (bx + cur_x)/2;
	my = (by + cur_y)/2;
	nx = ax*2 - mx;
	ny = ay*2 - my;
	
	for (int i = 0; i < segment; i++) {
		ratio = (i+1)/(double)segment;
		px = (nx - cur_x)*ratio + cur_x;
		py = (ny - cur_y)*ratio + cur_y;
		qx = (bx - nx)*ratio + nx;
		qy = (by - ny)*ratio + ny;
		tar_enqueue(((qx - px)*ratio + px), ((qy - py)*ratio + py), 0);
	}
}
*/

int main(void)
{
	led_init();
	tft_init(2, BLUE, WHITE, BLACK);
	encoder_init();
	gyro_init();
	buzzer_init();
	
	uart_init(COM1, 115200);
	uart_interrupt(COM1);
	bluetooth_init();
	
	can_init();
	can_rx_init();
	can_motor_init();
	
	_delay_ms(4000);
	
	tar_head = 0;
	tar_end = 0;
	err_d = 0;
	temp = 0;
	ticks_init();
	start = 0;
	
	tar_enqueue(0, 1000, 0, false);
	tar_enqueue(0, 2000, 0, true);
	
	while (1) {
		if (get_ticks() % 50 == 0) {
			
			//update variables
			passed = get_ticks() - start;
			cur_deg = get_angle();
			cor_x = 52;
			cor_y = 380;
			xy_rotate(&cor_x, &cor_y, (-1)*cur_deg);
			cur_x = get_X() + cor_x - 52;
			cur_y = get_Y() + cor_y - 380;
			
			degree = tar_dir - (cur_deg/10);
			degree_diff = tar_deg - (int)(cur_deg/10);
			degree_diff = (degree_diff+1080)%360;
			if (degree_diff > 180)
				degree_diff -= 360;
			
			dist = Sqrt(Sqr(tar_x - cur_x) + Sqr(tar_y - cur_y));
			
			can_motor_update();
			//can_calibrate();
		}
	}
}

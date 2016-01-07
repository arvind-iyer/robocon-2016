#include "main.h"
#include <math.h>
#include <stdbool.h>

#define THRESHOLD 50
#define KP 0.3

int tar_x, tar_y, tar_deg, cur_x, cur_y, cur_deg;
int cor_x, cor_y;
int v1, v2, v3;
int degree, degree_diff, dist, speed;
int start, passed;

void can_motor_set_angle(int angle, int rotate, int maxvel, int time)
{
	double accvel = 1.0;
	if (time < 2000)
		accvel = time/2000.0;
	rotate = (int)(rotate*KP);
	if ((rotate < 10) && (rotate > 0))
		rotate = 10;
	if ((rotate > -10) && (rotate < 0))
		rotate = -10;
	
	angle *= 10;
	v1 = int_sin(angle)*maxvel/(-10000);
	v2 = int_sin(angle+1200)*maxvel/(-10000);
	v3 = int_sin(angle+2400)*maxvel/(-10000);
	
	v1 -= rotate;
	v2 -= rotate;
	v3 -= rotate;	
	motor_set_vel(MOTOR1, v1*accvel, CLOSE_LOOP);
	motor_set_vel(MOTOR2, v2*accvel, CLOSE_LOOP);
	motor_set_vel(MOTOR3, v3*accvel, CLOSE_LOOP);
}

void can_motor_stop(){
	v1 = 0;
	v2 = 0;
	v3 = 0;
	motor_set_vel(MOTOR1, v1, CLOSE_LOOP);
	motor_set_vel(MOTOR2, v2, CLOSE_LOOP);
	motor_set_vel(MOTOR3, v3, CLOSE_LOOP);
	motor_lock(MOTOR1);
	motor_lock(MOTOR2);
	motor_lock(MOTOR3);
}

int main(void)
{
	led_init();
	tft_init(2, BLUE, WHITE, BLACK);
	ticks_init();
	encoder_init();
	gyro_init();
	buzzer_init();
	
	uart_init(COM1, 115200);
	uart_interrupt(COM1);
	bluetooth_init();
	
	can_init();
	can_rx_init();
	can_motor_init();
	
	tar_x = 0;
	tar_y = 0;
	tar_deg = 0;
	_delay_ms(4000);
	//set initial target pos
	tar_x = 0;
	tar_y = 1500;
	tar_deg = 120;
	start = get_ticks();
	
	while (1) {
		if (get_ticks() % 50 == 0) {
			passed = get_ticks() - start;
			
			cur_deg = get_angle();
			cor_x = 0;
			cor_y = 375;
			xy_rotate(&cor_x, &cor_y, (-1)*cur_deg);
			cur_x = get_X() + cor_x;
			cur_y = get_Y() + cor_y;
			
			degree = 90 - int_arc_tan2(tar_y - cur_y, tar_x - cur_x) - (int)(cur_deg/10);
			degree = (degree+1080)%360;
			degree_diff = tar_deg - (int)(cur_deg/10);
			degree_diff = (degree_diff+1080)%360;
			if (degree_diff > 180)
				degree_diff -= 360;
			
			dist = Sqrt(Sqr(tar_x - cur_x) + Sqr(tar_y - cur_y));
			
			/*
			//Self-rotation to measure correction vector
			if (cur_deg <= 1800) {
				motor_set_vel(MOTOR1, -20, CLOSE_LOOP);
				motor_set_vel(MOTOR2, -20, CLOSE_LOOP);
				motor_set_vel(MOTOR3, -20, CLOSE_LOOP);
			} else {
				can_motor_stop();
			}
			*/
			
			if ((Abs(tar_x - cur_x) < THRESHOLD) && (Abs(tar_y - cur_y) < THRESHOLD) && (Abs(tar_deg - cur_deg) < 10)) {
				can_motor_stop();
			} else {
				//decelerate
				if (dist < 450) {
					speed = (int)(dist/6);
				} else {
					speed = 75;
				}
				can_motor_set_angle(degree, degree_diff, speed, passed);
			}
						
			tft_clear();
			tft_prints(0,0,"X:%5d Y:%5d",cur_x,cur_y);
			tft_prints(0,1,"ANGLE %d",cur_deg);
			tft_prints(0,3,"X:%5d Y:%5d",tar_x,tar_y);
			tft_prints(0,4,"%5dmm  %3dd",dist,degree);
			tft_prints(0,5,"ROTATE %3dd",degree_diff);
			tft_prints(0,7,"VEL %3d %3d %3d",v1,v2,v3);
			tft_prints(0,8,"MAX %3d",speed);
			tft_prints(0,9,"TIM %3d",get_seconds());
			tft_update();
		}
	}
}

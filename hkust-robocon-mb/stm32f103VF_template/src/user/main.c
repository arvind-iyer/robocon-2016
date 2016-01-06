#include "main.h"
#include <math.h>
#include <stdbool.h>

#define THRESHOLD 50

int tar_x, tar_y, cur_x, cur_y, cur_deg;
int v1, v2, v3;
int degree, dist, speed;
bool stopped;

void can_motor_set_angle(int angle, int maxvel)
{
	angle = angle * 10;
	v1 = int_sin(angle)*maxvel/(-10000);
	v2 = int_sin(angle+1200)*maxvel/(-10000);
	v3 = int_sin(angle+2400)*maxvel/(-10000);
	motor_set_vel(MOTOR1, v1, CLOSE_LOOP);
	motor_set_vel(MOTOR2, v2, CLOSE_LOOP);
	motor_set_vel(MOTOR3, v3, CLOSE_LOOP);
}

void can_motor_stop(){
	v1 = 0;
	v2 = 0;
	v3 = 0;
	motor_lock(MOTOR1);
	motor_lock(MOTOR2);
	motor_lock(MOTOR3);
}

//deceleration

int main(void)
{
	led_init();
	tft_init(2, BLUE, WHITE, BLACK);
	ticks_init();
	encoder_init();
	gyro_init();
	
	uart_init(COM1, 115200);
	uart_interrupt(COM1);
	bluetooth_init();
	
	can_init();
	can_rx_init();
	can_motor_init();
	
	tar_x = 0;
	tar_y = 0;
	_delay_ms(2000);
	tar_x = 0;
	tar_y = 2000;
	stopped = false;
	
	while (1) {
		if (get_ticks() % 50 == 0) {
			cur_x = get_X();
			cur_y = get_Y();
			cur_deg = get_angle();
			degree = 90 - int_arc_tan2(tar_y - cur_y, tar_x - cur_x) - (int)(cur_deg/10);
			dist = Sqrt(Sqr(tar_x - cur_x) + Sqr(tar_y - cur_y));
			
			if ((Abs(tar_x - cur_x) < THRESHOLD) && (Abs(tar_y - cur_y) < THRESHOLD)) {
				can_motor_stop();
			} else {
				if (dist < 300) {
					speed = (int)(dist/6);
				} else {
					speed = 50;
				}
				can_motor_set_angle(degree, speed);
			}
			
			tft_clear();
			tft_prints(0,0,"CUR X: %d",cur_x);
			tft_prints(0,1,"CUR Y: %d",cur_y);
			tft_prints(0,2,"ANGLE: %d",cur_deg);
			tft_prints(0,3,"DEGREE: %d",degree);
			tft_prints(0,4,"DIST: %d",dist);
			tft_prints(0,5,"V1: %d",v1);
			tft_prints(0,6,"V2: %d",v2);
			tft_prints(0,7,"V3: %d",v3);
			tft_prints(0,8,"TIMER: %d",get_seconds());
			tft_update();
		}
	}
}

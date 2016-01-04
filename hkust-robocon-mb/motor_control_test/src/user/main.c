#include "main.h"
#include "math.h"
#define PI 3.14159265
#define MAXVEL 50


u16 ticks_img 	= (u16)-1;
u16 seconds_img = (u16)-1;

u16 ticks_last = 0;
u8 state = 0;
u8 string_pointer = 0;
u8 input_string[128];

void can_motor_set_angle(int angle)
{
	motor_set_vel(MOTOR1, sin((angle)*PI/180)*MAXVEL*(-1), CLOSE_LOOP);
	motor_set_vel(MOTOR2, sin((angle+120)*PI/180)*MAXVEL*(-1), CLOSE_LOOP);
	motor_set_vel(MOTOR3, sin((angle+240)*PI/180)*MAXVEL*(-1), CLOSE_LOOP);
}

void can_motor_stop(){
	motor_set_vel(MOTOR1, 0, CLOSE_LOOP);
	motor_set_vel(MOTOR2, 0, CLOSE_LOOP);
	motor_set_vel(MOTOR3, 0, CLOSE_LOOP);
}

int main(void){
	tft_init(2, BLACK, WHITE, RED);
	ticks_init();
	gyro_init();
	can_init();
	can_rx_init();
	can_motor_init();
	
	setTarget(200, 200);
	
	can_motor_set_angle(200);
	
	while (1) {
		map();
//				tft_clear();
//				tft_prints(0,0,"X: %d", get_X());
//				tft_prints(0,1,"Y: %d", get_Y());
//				tft_prints(0,2,"Angle: %d", get_angle());
//				tft_prints(0, 4, "En. 1: %d", get_encoder_value(MOTOR1));
//				tft_prints(0, 5, "En. 2: %d", get_encoder_value(MOTOR2));
//				tft_prints(0, 6, "En. 3: %d", get_encoder_value(MOTOR3));
//				tft_update();
		
		if (get_seconds()>2){
			can_motor_stop();
		}
	}
}

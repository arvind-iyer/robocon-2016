#include "main.h"
#include "math.h"
#include <stdbool.h>
#define PI 3.14159265
#define MAXVEL 50
#define e 20

//34
//37

u16 ticks_img=(u16)-1;
u16 seconds_img=(u16)-1;

u16 ticks_last=0;
u8 state=0;
u8 string_pointer=0;
u8 input_string[128];

int targetX;
int targetY;
int startX;
int startY;

void can_motor_set_angle(int angle)
{
	motor_set_vel(MOTOR1, sin((angle)*PI/180)*MAXVEL*(-1), CLOSE_LOOP);
	motor_set_vel(MOTOR2, sin((angle+120)*PI/180)*MAXVEL*(-1), CLOSE_LOOP);
	motor_set_vel(MOTOR3, sin((angle+240)*PI/180)*MAXVEL*(-1), CLOSE_LOOP);
}

void can_motor_stop()
{
	motor_set_vel(MOTOR1, 0, CLOSE_LOOP);
	motor_set_vel(MOTOR2, 0, CLOSE_LOOP);
	motor_set_vel(MOTOR3, 0, CLOSE_LOOP);
}

int getX()
{
	get_X();
}

int getY()
{
	get_Y();
}

void setTarget(int x, int y)
{
	targetX=x;
	targetY=y;
	startX=getX();
	startY=getY();
}

int main(void)
{
	button_init();
	tft_init(2, BLACK, WHITE, RED);
	ticks_init();
	gyro_init();
	can_init();
	can_rx_init();
	can_motor_init();
	button_init();
	setTarget(34, 37);
	
	int target_direction = (int_arc_tan2(targetY - getY(), targetX - getX()) - get_angle()/10) %360;
	
	bool button=false;
	
	while (1)
	{
		tft_clear();
		tft_prints(0,0,"X: %d", get_X());
		tft_prints(0,1,"Y: %d", get_Y());
		tft_prints(0,2,"Angle: %d", get_angle());
		tft_prints(0, 4, "En. 1: %d", get_encoder_value(MOTOR1));
		tft_prints(0, 5, "En. 2: %d", get_encoder_value(MOTOR2));
		tft_prints(0, 6, "En. 3: %d", get_encoder_value(MOTOR3));
		tft_prints(0, 7, "T: %d", target_direction);
		tft_update();
		if (button_pressed(BUTTON_1)>5)
		{
			button=true;
		}
		if (button)
		{
		if (getX()>=targetX && getY()>=targetY)
		{
			can_motor_stop();
		}
		else
		{
			/*float m;
			float c;
			m=(targetY-targetX)/(startY-startX);
			c=targetY-m*targetX;
			float Dx;
			float Dy;
			Dx=-(getY()-targetX+getX()/m+targetY*m)/(m+1/m);
			Dy=m*Dx+c;
			*/
			
			
			can_motor_set_angle(target_direction);
		}
		}
	}
}

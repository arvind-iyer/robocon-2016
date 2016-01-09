#include "main.h"
#include <stdlib.h>

#define PI 3.1415926535897932
#define MAXVEL 140

int M1;
int M2;
int M3;
int target_x;
int target_y;
int target_angle;
int target_direction=0;
int dist=0;
int W;
int M;

void _updateScreen();
void _path(int x, int y, int angle);
void _move(float magnitude, float bearing, float W);
void _setTarget(int x, int y, int angle);
int _getX();
int _getY();

int main()
{
	tft_init(2, BLACK, WHITE, RED);
	ticks_init();
	gyro_init();
	can_init();
	can_rx_init();
	can_motor_init();
	while (1)
	{
		_path(250, 250, 0);
		_updateScreen();
	}
}

void _updateScreen()
{
	tft_clear();
	tft_prints(0,0,"X: %d", _getX());
	tft_prints(0,1,"Y: %d", _getY());
	tft_prints(0,2,"Angle: %d", get_angle());
	tft_prints(0, 3, "_move(%d, %d, %d)", M, target_direction, W);
	tft_prints(0, 4, "MOTOR: %d %d %d", M1, M2, M3);
	tft_prints(0, 5, "Target: %d %d", target_x, target_y);
	tft_prints(0, 6, "t_dir: %d", target_direction);
	tft_prints(0, 7, "dis: %d", dist);
	tft_prints(0, 9, "timer: %d", get_ticks());
	tft_update();
}

void _path(int x, int y, int angle)
{
	_setTarget(x, y, angle);
	while (dist>100 || Abs(target_angle-get_angle())>15)
	{
		target_direction = (int_arc_tan2(target_y - _getY(), target_x - _getX()) - get_angle()/10) %360;
		target_direction = target_direction < 0 ? target_direction + 360 : target_direction;
		dist=Sqrt(Sqr(target_y-_getY())+Sqr(target_x-_getX()));
		if (target_angle-angle>Abs(target_angle-angle))
		{
			W=360-(target_angle-angle);
		}
		else
		{
			W=target_angle-angle;
		}
		if (dist/10+10>100)
		{
			M=100;
		}
		else
		{
			M=dist/10+10;
		}
		_move(M, target_direction, W);
		_updateScreen();
	}
	motor_lock(MOTOR1);
	motor_lock(MOTOR2);
	motor_lock(MOTOR3);
}

void _move(float magnitude, float bearing, float W)
{
	if (magnitude>100)
	{
		magnitude=100;
	}
	float X;
	float Y;
	X=magnitude*int_sin(bearing)*MAXVEL/100/10000;
	Y=magnitude*int_cos(bearing)*MAXVEL/100/10000;
	if (Sqrt(X*X+Y*Y)>MAXVEL)
	{
		Y=Sqrt(MAXVEL*MAXVEL/(1+X*X/Y/Y));
		X=Sqrt(MAXVEL*MAXVEL-Y*Y);
	}
	M1=(-W-X*2)/3;
	M2=(-W*Sqrt(3)/3+X*Sqrt(3)/3-Y)/Sqrt(3);
	M3=-W-M1-M2;
	//motor control
	motor_set_vel(MOTOR1, M1, CLOSE_LOOP);
	motor_set_vel(MOTOR2, M2, CLOSE_LOOP);
	motor_set_vel(MOTOR3, M3, CLOSE_LOOP);
}

void _setTarget(int x, int y, int angle)
{
	target_x=x;
	target_y=y;
	target_angle=angle;
}

int _getX()
{
	return -get_pos()->y;
}

int _getY()
{
	return get_pos()->x;
}

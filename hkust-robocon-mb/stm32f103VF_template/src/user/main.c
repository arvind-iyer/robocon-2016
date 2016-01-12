#include "main.h"
#include <stdlib.h>

#define PI 3.1415926535897932
#define MAXVEL 140

int _M1;
int _M2;
int _M3;
int _x;
int _y;
int _angle;

int dist_e;
int angle_e;

int x_f;
int y_f;

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
float err=1;

void _updateScreen();
void _updateStatus();
void _checkpoint(int x, int y, int angle, int d_e, int a_e);
void _path();
void _move(float magnitude, float bearing, float W);
void _setTarget(int x, int y, int angle, int d_e, int a_e);
void _setFinal(int x, int y);
int _getX();
int _getY();
void _delay();
int _angleDiff(int o, int t);
void _curve(int x1, int y1, int x2, int y2);

int main()
{
	tft_init(2, BLACK, WHITE, RED);
	ticks_init();
	gyro_init();
	can_init();
	can_rx_init();
	can_motor_init();
	_delay();
	
	_setFinal(1000, 1000);
	_curve(0, 1000, 1000, 1000);
	while (1)
	{
		_checkpoint(1000, 1000, 0, 20, 5);
		_updateScreen();
	}
}

void _updateScreen()
{
	tft_clear();
	tft_prints(0, 0, "X: %d", _getX());
	tft_prints(0, 1, "Y: %d", _getY());
	tft_prints(0, 2, "Angle: %d", get_angle());
	tft_prints(0, 3, "_m(%d, %d, %d)", M, target_direction, W);
	tft_prints(0, 4, "M: %d %d %d", M1, M2, M3);
	tft_prints(0, 5, "err: %.2f", err); 
	tft_prints(0, 6, "T: %d %d %d", target_x, target_y, target_angle);
	tft_prints(0, 7, "t_dir: %d", target_direction);
	tft_prints(0, 8, "dis: %d", dist);
	tft_prints(0, 9, "timer: %d", get_ticks());
	tft_update();
}

void _updateStatus()
{
	dist=Sqrt(Sqr(target_y-_getY())+Sqr(target_x-_getX()));
	target_direction = ((90-(int_arc_tan2(target_y - _getY(), target_x - _getX()))) - get_angle()/10) %360;
	target_direction = target_direction < 0 ? target_direction + 360 : target_direction;
}

void _checkpoint(int x, int y, int angle, int d_e, int a_e)
{
	_setTarget(x, y, angle, d_e, a_e);
	_updateStatus();
	_updateScreen();
	_path();
}

void _path()
{
	while (dist>dist_e || Abs(target_angle-get_angle()/10)>angle_e)
	{
		int dist_f;
		dist_f=Sqrt(Sqr(y_f-_getY())+Sqr(x_f-_getX()));
		_updateStatus();
		W=_angleDiff(get_angle()/10, target_angle)*100/360;
		if (dist_f/20>100)
		{
			M=100;
		}
		else
		{
			M=dist_f/20;
		}
		if (dist>0 && M==0)
		{
			M=1;
		}
		if (_angleDiff(get_angle()/10, target_angle)>0 && W==0)
		{
			W=1;
		}
		if (_angleDiff(get_angle()/10, target_angle)<0 && W==0)
		{
			W=-1;
		}
		_updateScreen();
		_move(M, target_direction*10, W);
	}
	M=0;
	W=0;
	_move(M, target_direction*10, W);
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
	_M1=(-W-X*2)/3;
	_M2=(-W*Sqrt(3)/3+X*Sqrt(3)/3-Y)/Sqrt(3);
	_M3=-W-M1-M2;
	if ((magnitude!=0 || W!=0) && _x==_getX() && _y==_getY() && _angle==get_angle())
	{
		if (_M1*(err+0.03)<=140 && _M2*(err+0.03)<=140 && _M3*(err+0.03)<=140)
		{
			err=err+0.03;
		}
	}
	else
	{
		err=err-0.13;
		if (err<1)
		{
			err=1;
		}
	}
	M1=((-W-X*2)/3)*err;
	M2=((-W*Sqrt(3)/3+X*Sqrt(3)/3-Y)/Sqrt(3))*err;
	M3=(-W-M1-M2)*err;
	//motor control
	motor_set_vel(MOTOR1, M1, CLOSE_LOOP);
	motor_set_vel(MOTOR2, M2, CLOSE_LOOP);
	motor_set_vel(MOTOR3, M3, CLOSE_LOOP);
	_x=_getX();
	_y=_getY();
	_angle=get_angle();
}

void _setTarget(int x, int y, int angle, int d_e, int a_e)
{
	dist_e=d_e;
	angle_e=a_e;
	target_x=x;
	target_y=y;
	target_angle=angle;
}

void _setFinal(int x, int y)
{
	x_f=x;
	y_f=y;
}

int _getX()
{
	return -get_pos()->y;
}

int _getY()
{
	return get_pos()->x;
}

void _delay()
{
	int start=get_seconds();
	while (get_seconds()-start!=5)
	{
		_updateScreen();
		motor_lock(MOTOR1);
		motor_lock(MOTOR2);
		motor_lock(MOTOR3);
	}
}

int _angleDiff(int o, int t)
{
	int a=t-o;
	if (0>a && Abs(a)>180)
	{
		a=a+360;
	}
	return a;
}

void _curve(int x1, int y1, int x2, int y2)
{
	int x0;
	int y0;
	x0=_getX();
	y0=_getY();
	for (float t=0; t<=1; t=t+0.1)
	{
		int x;
		int y;
		x=(1-t)*(1-t)*x0+2*(1-t)*t*x1+t*t*x2;
		y=(1-t)*(1-t)*y0+2*(1-t)*t*y1+t*t*y2;
		_checkpoint(x, y, 0, 100, 360);
	}
}
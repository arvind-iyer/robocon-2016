#include "main.h"
#include <stdlib.h>

#define MAXVEL 140
#define PI 3.1415926535897932

int M;
int dir;
int W;

int _x;
int _y;
int _angle;

int M1;
int M2;
int M3;

int start_x;
int start_y;
int target_x;
int target_y;
int target_angle;

int end_vel;

float err;

void _updateScreen();
void _move(int M, int dir, int W);

void _setTarget(int x, int y, int bearing);
void _straight(int x, int y, int target_angle, int d_e, int a_e, int vel);
void _curve(int x, int y, int target_angle, int bend);

int _dist(int x0, int y0, int x, int y);
int _angleDiff(int o, int t);
int _bearing(int x0, int y0, int x, int y);
int _pidBearing();

void _delay();
int _getX();
int _getY();
int _getAngle();

int main()
{
	tft_init(2, BLACK, WHITE, RED);
	ticks_init();
	gyro_init();
	can_init();
	can_rx_init();
	can_motor_init();
	_delay();
	while (1)
	{
		_updateScreen();
		
	}
}

void _updateScreen()
{
}

void _move(int M, int dir, int W)
{
	float _M1;
	float _M2;
	float _M3;
	float X;
	float Y;
	X=M*int_sin(dir)*MAXVEL/100/10000;
	Y=M*int_cos(dir)*MAXVEL/100/10000;
	if (Sqrt(X*X+Y*Y)>MAXVEL)
	{
		Y=Sqrt(MAXVEL*MAXVEL/(1+X*X/Y/Y));
		X=Sqrt(MAXVEL*MAXVEL-Y*Y);
	}
	_M1=(-W-X*2)/3;
	_M2=(-W*Sqrt(3)/3+X*Sqrt(3)/3-Y)/Sqrt(3);
	_M3=-W-M1-M2;
	if ((M!=0 || W!=0) && _x==_getX() && _y==_getY() && _angle==get_angle())
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

void _setTarget(int x, int y, int bearing)
{
	target_x=x;
	target_y=y;
	target_angle=bearing;
}

void _straight(int x, int y, int target_angle, int d_e, int a_e, int vel)
{
	int distance;
	distance=_dist(_getX(), _getY(), x, y);
	while (distance>d_e || Abs(target_angle-_getAngle()/10)>a_e)
	{
		M=_dist(_getX(), _getY(), x, y)/20;
		if (M>100)
		{
			M=100;
		}
		if (_dist(_getX(), _getY(), x, y)>0 && M==0)
		{
			M=1;
		}
		dir=_pidBearing();
		W=_angleDiff(_getAngle()/10, target_angle)*50/360;
		_move(M, dir, W);
	}
	M=0;
	W=0;
	_move(M, dir, W);
}

void _curve(int x, int y, int target_angle, int bend)
{
	float centerX=target_x-start_x;
	float centerY=target_y-start_y;
	float radius=Sqrt(Sqr(centerX)+Sqr(centerY))/2;
	float angle=int_arc_tan2(centerY, centerX)*10+900+bend;
	for (float t=0; t!=1; t=t+0.2)
	{
		int c_x=(1-t)*(1-t)*start_x+2*(1-t)*t*centerX+t*t*target_x;
		int c_y=(1-t)*(1-t)*start_y+2*(1-t)*t*centerY+t*t*target_y;
		_straight(c_x, c_y, 0, 100, 360, 20);
	}
	_straight(x, y, 0, 100, 360, 0);
}

int _dist(int x0, int y0, int x, int y)
{
	int dist;
	dist=Sqrt(Sqr(x-x0)+Sqr(y-y0));
	return dist;
}

int _bearing(int x0, int y0, int x, int y)
{
	int a;
	a=(90-(int_arc_tan2(y - y0, x - x0)))%360;
	if (0>a)
	{
		a=a+360;
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

int _pidBearing()
{
	float diffTargetY=target_y-_getY();
	float diffTargetX=target_x-_getX();
	float angleToTarget=int_arc_tan2(diffTargetY, diffTargetX)*10;
	float targetDst=Sqrt(Sqr(diffTargetY)+Sqr(diffTargetX));
	float diffLineX=target_x-start_x;
	float diffLineY=target_y-start_y;
	float u=((_getX()-start_x)*diffLineX+(_getY()-start_y)*diffLineY)/((float)diffLineX*diffLineX+diffLineY*diffLineY);
	float linePointX=start_x+u*diffLineX;
	float linePointY=start_y+u*diffLineY;
	float diffPointX=linePointX-_getX();
	float diffPointY=linePointY-_getY();
	float dstToPoint=(float)Sqrt(Sqr(diffPointX)+Sqr(diffPointY));
	float angleToLine=int_arc_tan2(diffPointY, diffPointX)*10;
	return ((900-int_arc_tan2(targetDst*int_sin(angleToTarget)+dstToPoint*int_sin(angleToLine), targetDst*int_cos(angleToTarget)+dstToPoint*int_cos(angleToLine))*10)-_getAngle())/10;
}

void _delay()
{
	int start=get_seconds();
	while (get_seconds()-start!=5)
	{
		_updateScreen();
	}
}

int _getX()
{
	//return get_pos()->x;
	return -get_pos()->y;
}

int _getY()
{
	//return get_pos()->y;
	return get_pos()->x;
}

int _getAngle()
{
	return get_angle();
}
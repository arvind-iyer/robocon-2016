#include "main.h"
#include <stdlib.h>

int M;
int dir;
int W;

int start_x;
int start_y;
int target_x;
int target_y;
int end_vel;

float err;

void _move(int M, int dir, int W);

void _straight(int x, int y, int target_angle, int d_e, int a_e);
void _curve(int x, int y, int target_angle, int bend);

int _bearing(int x0, int y0, int x, int y);
int _pidBearing();

void _delay();
int _getX();
int _getY();
int _getAngle();

int main()
{
	
}

void _move(int M, int dir, int W)
{
}

void _straight(int x, int y, int target_angle, int d_e, int a_e)
{
	
}

void _curve(int x, int y, int target_angle, int bend)
{
	float centerX=target_x-start_x;
	float centerY=target_y-start_y;
	float radius=Sqrt(Sqr(centerX)+Sqr(centerY))/2;
	float angle=int_arc_tan2(centerY, centerX)*10+900+bend;
	for (float t=0; t!=1; t=t+0.2)
	{
		int _x;
		int _y;
		_x=(1-t)*(1-t)*start_x+2*(1-t)*t*centerX+t*t*target_x;
		_y=(1-t)*(1-t)*start_y+2*(1-t)*t*centerY+t*t*target_y;
		_straight(_x, _y, 0, 100, 360);
	}
	_straight(x, y, 0, 100, 360);
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

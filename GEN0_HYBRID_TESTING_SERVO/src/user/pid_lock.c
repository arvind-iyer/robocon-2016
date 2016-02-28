#include "pid_lock.h"

int M=0;
int dir=0;
int W=0;
int lock=0;
int pneu=0;
int x_m;
int y_m;
int dir_m;

int _x=0;
int _y=0;
int _angle=0;
int _distance=0;
int _M=0;

int M1=0;
int M2=0;
int M3=0;

int start_x;
int start_y;
int target_x=0;
int target_y=0;
int target_angle=0;

int end_vel;

float err=1;


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

void _lockInTarget(){
	_straight(target_x, target_y, target_angle, 5, 5, 0);
}

void _setTarget(int x, int y, int bearing)
{
	err=1;
	target_x=x;
	target_y=y;
	target_angle=bearing; 
}



void _setCurrentAsTarget()
{
	err=1;
	target_x=_getX();
	target_y=_getY();
	target_angle=_getAngle()/10; 
}


int _dist(int x0, int y0, int x, int y)
{
	int dist;
	dist=Sqrt(Sqr(x-x0)+Sqr(y-y0));
	return dist;
}

int _atan2(int y, int x)
{
	return int_arc_tan2(y, x);
}

int _bearing(int x0, int y0, int x, int y)
{
	int bearing;
	bearing=90-_atan2(y-y0, x-x0);
	if (bearing<0)
	{
		bearing=bearing+360;
	}
	return bearing;
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

int _vectorAdd(int m1, int b1, int m2, int b2)
{
	return _bearing(0, 0, m1*int_sin(b1*10)/10000+m2*int_sin(b2*10)/10000, m1*int_cos(b1*10)/10000+m2*int_cos(b2*10)/10000);
}

void _move(int M, int dir, int W)
{
	if (M>100)
	{
		M=100;
	}
	float _M1;
	float _M2;
	float _M3;
	float X;
	float Y;
	X=M*int_sin(dir*10)*MAXVEL/100/10000;
	Y=M*int_cos(dir*10)*MAXVEL/100/10000;
	_M1=(-W-X*2)/3;
	_M2=(-W*0.577f+X*0.577f-Y)/1.73f;
	_M3=-W-_M1-_M2;
	
	if ((M!=0 || W!=0) && (_dist(_getX(), _getY(), target_x, target_y)>=_distance && Abs(_getAngle()/10-target_angle)>Abs(_angle/10-target_angle)))
	{
		if (Abs(_M1*(err+0.03))<MAXVEL && Abs(_M2*(err+0.03))<MAXVEL && Abs(_M3)<MAXVEL)
		{
			err=err+0.03;
		}
	}
	else if (_distance>_dist(_getX(), _getY(), target_x, target_y))
	{
		err=err*0.8+0.2;
	}
	
	M1=_M1*err;
	M2=_M2*err;
	M3=-W-M1-M2;
	//motor control
	motor_set_vel(MOTOR1, M1, CLOSE_LOOP);
	motor_set_vel(MOTOR2, M2, CLOSE_LOOP);
	motor_set_vel(MOTOR3, M3, CLOSE_LOOP);
	//update
	_x=_getX();
	_y=_getY();
	_angle=_getAngle();
	_distance=_dist(_getX(), _getY(), target_x, target_y);
}


void _straight(int x, int y, int bearing, int d_e, int a_e, int vel)
{
	int distance;
	distance=_dist(_getX(), _getY(), x, y);
	if (distance>d_e || Abs(bearing-_getAngle()/10)>a_e){
		distance=_dist(_getX(), _getY(), x, y);
		if (vel!=0)
		{
			M=vel;
		}
		else
		{
			M=distance/20;
		}
		if (M>100)
		{
			M=100;
		}
		if (distance>d_e && M==0)
		{
			M=1;
		}
		if (distance>200 && lock%2==0)
		{
			int dir1;
			dir1 = ((90-(int_arc_tan2(target_y - _getY(), target_x - _getX()))) - _getAngle()/10) %360;
			if (dir1<0)
			{
				dir1=dir1+360;
			}
			//pid
			float px=x-start_x;
			float py=y-start_y;
			float dab=px*px+py*py;
			float u=((_getX()-start_x)*px+(_getY()-start_y)*py)/dab;
			dir1=_vectorAdd(_dist(_getX(), _getY(), start_x+u*px, start_y+u*py), _bearing(_getX(), _getY(), start_x+u*px, start_y+u*py), distance, dir1);
			dir=(dir1-_getAngle()/10)%360;
			if (dir<0)
			{
				dir=dir+360;
			}
		}
		else
		{
			//dir=(_bearing(x, y, _getX(), _getY())-_getAngle()/10)%360;
			dir = ((90-(int_arc_tan2(target_y - _getY(), target_x - _getX()))) - _getAngle()/10) %360;
			if (dir<0)
			{
				dir=dir+360;
			}
		}
		W=_angleDiff(_getAngle()/10, bearing)*100/360;
		if (Abs(_angleDiff(_getAngle()/10, bearing))>a_e && W==0)
		{
			if (_angleDiff(_getAngle()/10, bearing)>0)
			{
				W=1;
			}
			else
			{
				W=-1;
			}
		}
		if (M!=1 && Abs(M-_M)>5)
		{
			M=M*0.2f+_M*0.8f;
		}
		_M=M;
		_move(M, dir, W);
	}
	if (lock%2==0 || (distance<d_e && Abs(bearing-_getAngle()/10)<a_e))
	{
		err=1;
		M=0;
		W=0;
		_move(M, dir, W);
	}
}

#include "main.h"
#include <stdlib.h>

#define MAXVEL 140
#define PI 3.1415926535897932

//debug
void _TEST_8figure();
//end

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

float err=0;

void _updateScreen();
void _move(int M, int dir, int W);

void _setTarget(int x, int y, int bearing);
void _straight(int x, int y, int bearing, int d_e, int a_e, int vel);
void _curve(int x, int y, int centerX, int centerY);

int _dist(int x0, int y0, int x, int y);
int _angleDiff(int o, int t);
int _bearing(int x0, int y0, int x, int y);
int _vectorAdd(int m1, int b1, int m2, int b2);

void _manual();
int _manualCheck();

void _brushlessStartup();

void _delay(int sec);
int _getX();
int _getY();
int _getAngle();

int main()
{
	tft_easy_init();
	//tft_init(2, BLACK, WHITE, RED);
	ticks_init();
	gyro_init();
	can_init();
	can_rx_init();
	can_motor_init();
	_delay(1);
	xbc_mb_init(XBC_CAN_FIRST);
	servo_init();
	pneumatic_init();
	_brushlessStartup();
	_delay(5);
	//start
	lock=0;
	while (1)
	{
		_updateScreen();
		_manual();
		//_straight(0, 1000, 0, 5, 5, 0);
	}
	
}

//debug
void _TEST_8figure()
{
	_curve(0, 500, 750, 250);
	_curve(0, 1000, -750, 750);
	_curve(0, 500, 750, 750);
	_curve(0, 0, -750, 250);
}
//end

void _updateScreen()
{
	int bear;
	bear=_bearing(_getX(), _getY(), target_x, target_y)-_getAngle()/10;
	if (0>bear)
	{
		bear=bear+360;
	}
	tft_clear();
	tft_prints(16, 0, "%d", get_ticks());
	tft_prints(0, 0, "%d %d [%d]", _getX(), _getY(), _getAngle()/10);
	tft_prints(0, 1, "M: ([%.2f]) %d %d %d", err, M1, M2, M3);
	tft_prints(0, 2, "_m(%d, %d, %d)", M, dir, W);
	tft_prints(0, 3, "Q: %d %d %d", target_x, target_y, target_angle);
	tft_prints(0, 4, "T: %d [%d]", _dist(_getX(), _getY(), target_x, target_y), bear);
	//tft_prints(0, 5, "DEBUG: %d %d", xbc_get_joy(XBC_JOY_LT), xbc_get_joy(XBC_JOY_RT));
	if (lock%2==1)
	{
		tft_prints(0, 6, "[LOCK]");
	}
	tft_update();
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
	_updateScreen();
	motor_set_vel(MOTOR1, M1, CLOSE_LOOP);
	motor_set_vel(MOTOR2, M2, CLOSE_LOOP);
	motor_set_vel(MOTOR3, M3, CLOSE_LOOP);
	//update
	_x=_getX();
	_y=_getY();
	_angle=_getAngle();
	_distance=_dist(_getX(), _getY(), target_x, target_y);
}

void _setTarget(int x, int y, int bearing)
{
	err=1;
	target_x=x;
	target_y=y;
	target_angle=bearing; 
}

void _straight(int x, int y, int bearing, int d_e, int a_e, int vel)
{
	int distance;
	distance=_dist(_getX(), _getY(), x, y);
	while (distance>d_e || Abs(bearing-_getAngle()/10)>a_e)
	{
		if (_manualCheck()==1)
			break;
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
		_updateScreen();
		_move(M, dir, W);
		if (lock%2==1)
			break;
	}
	if (lock%2==0 || (distance<d_e && Abs(bearing-_getAngle()/10)<a_e))
	{
		err=1;
		M=0;
		W=0;
		_move(M, dir, W);
	}
}

void _curve(int x, int y, int x1, int y1)
{
	int x0;
	int y0;
	x0=_getX();
	y0=_getY();
	for (float t=0; t<=1; t=t+0.1f)
	{
		if (_manualCheck()==1)
			break;
		int c_x=(1-t)*(1-t)*x0+2*(1-t)*t*x1+t*t*x;
		int c_y=(1-t)*(1-t)*y0+2*(1-t)*t*y1+t*t*y;
		_setTarget(c_x, c_y, 0);
		_straight(c_x, c_y, 0, 20, 360, 20);
	}
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

void _manual()
{
	button_update();
	if (_manualCheck()==1 || lock%2==0)
	{
		err=1;
	}
	//XBC_JOY_LX XBC_JOY_LY BUTTON_XBC_LB BUTTON_XBC_RB
	dir=int_arc_tan2(xbc_get_joy(XBC_JOY_LX), xbc_get_joy(XBC_JOY_LY));
	if (dir<0)
	{
		dir=dir+360;
	}
	M=(xbc_get_joy(XBC_JOY_LY)*xbc_get_joy(XBC_JOY_LY)+xbc_get_joy(XBC_JOY_LX)*xbc_get_joy(XBC_JOY_LX))/10000*30/100;
	if (button_pressed(BUTTON_XBC_RB))
	{
		W=30;
	}
	if (button_pressed(BUTTON_XBC_LB))
	{
		W=-30;
	}
	if (!button_pressed(BUTTON_XBC_RB) && !button_pressed(BUTTON_XBC_LB) && lock%2==0)
	{
		W=0;
	}
	if (_manualCheck()==1 || lock%2==0)
	{
		_move(M, dir, W);
	}
	//BUTTON_XBC_Y
	if (button_pressed(BUTTON_XBC_Y))
	{
		motor_set_vel(MOTOR4, 1799, OPEN_LOOP);
		motor_set_vel(MOTOR5, 1799, OPEN_LOOP);
		motor_set_vel(MOTOR6, 1799, OPEN_LOOP);
	}
	//BUTTON_XBC_A
	if (button_pressed(BUTTON_XBC_A))
	{
		motor_set_vel(MOTOR4, -600, OPEN_LOOP);
		motor_set_vel(MOTOR5, -600, OPEN_LOOP);
		motor_set_vel(MOTOR6, -600, OPEN_LOOP);
	}
	if (!button_pressed(BUTTON_XBC_Y) && !button_pressed(BUTTON_XBC_A))
	{
		motor_set_vel(MOTOR4, 0, OPEN_LOOP);
		motor_set_vel(MOTOR5, 0, OPEN_LOOP);
		motor_set_vel(MOTOR6, 0, OPEN_LOOP);
	}
	//BUTTON_XBC_X
	if (button_pressed(BUTTON_XBC_X))
	{
		_setTarget(_getX(), _getY(), _getAngle()/10);
		lock++;
		_delay(2);
	}
	if (lock%2==1 && _manualCheck()==0)
	{
		_straight(target_x, target_y, target_angle, 5, 5, 0);
	}
	//BUTTON_XBC_B
	if (button_pressed(BUTTON_XBC_B))
	{
		pneu++;
		pneumatic_control(GPIOB, GPIO_Pin_9, pneu%2);
		_delay(2);
	}
	//XBC_JOY_LT
	servo_control(SERVO1, 400+xbc_get_joy(XBC_JOY_LT)*650/255);
	servo_control(SERVO2, 400+xbc_get_joy(XBC_JOY_RT)*650/255);
	tft_prints(0, 5, "%d %d", 400+xbc_get_joy(XBC_JOY_LT)*650/255, 400+xbc_get_joy(XBC_JOY_RT)*650/255);
	tft_update();
}

int _manualCheck()
{
	button_update();
	if (xbc_get_joy(XBC_JOY_LX)!=0)
	{
		return 1;
	}
	if (xbc_get_joy(XBC_JOY_LY)!=0)
	{
		return 1;
	}
	if (button_pressed(BUTTON_XBC_LB))
	{
		return 1;
	}
	if (button_pressed(BUTTON_XBC_RB))
	{
		return 1;
	}
	return 0;
}

void _brushlessStartup()
{
	int timer;
	timer=get_seconds();
	while (get_seconds()-timer<=2)
	{
		servo_control(SERVO1, 400);
		servo_control(SERVO2, 400);
		tft_prints(0, 5, "400 400");
		tft_update();
	}
}

void _delay(int sec)
{
	int start=get_seconds();
	while (get_seconds()-start!=sec)
	{
		tft_prints(14, 5, "[DELAY]");
		tft_update();
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

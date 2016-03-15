#include "main.h"
//#include "pid.c"
#include <stdlib.h>

void _updateScreen();
void _delay(int sec);
int _getX();
int _getY();
int _getAngle();

int main()
{
	tft_easy_init();
	ticks_init();
	gyro_init();
	can_motor_init();
	_delay(5);
	//start
	while (1)
	{
		_updateScreen();
		
	}
	
}

void _updateScreen()
{
	tft_clear();
	tft_prints(16, 0, "%d", get_ticks());
	tft_prints(0, 0, "%d %d [%d]", _getX(), _getY(), _getAngle()/10);
	tft_update();
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

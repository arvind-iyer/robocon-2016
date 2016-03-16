#include "main.h"
#include "pid.h"
#include <stdlib.h>

int delayCount;

void _updateScreen();
void _delay(int sec);
int _getX();
int _getY();
int _getAngle();

int M1;
int M2;
int M3;

int queue = 0;

int main()
{
	delayCount = 0;
	tft_easy_init();
	ticks_init();
	gyro_init();
	can_init();
	can_motor_init();
	_delay(5);
	//start
	pid_init();
	
	Checkpoint target[] = {
		newCheckpoint(newPosition(newVector(0, 1000), 0), 0, 50, 5),
		newCheckpoint(newPosition(newVector(0, 2000), 180), 0, 50, 5),
		newCheckpoint(newPosition(newVector(0, 0), 180), 0, 50, 5),
		newCheckpoint(newPosition(newVector(0, 0), 0), 0, 50, 5)
	};
	lockTarget(target[0].position.vector, target[0].position.orientation, target[0].velocity);
	
	_delay(2);
	
	while (1)
	{
		_updateScreen();
		updateCurrentPos(newPosition(newVector(_getX(), _getY()), _getAngle() / 10));
		generatePID_M();
		generatePID_bearing();
		generatePID_W();
		if (!completed(target[queue].dist_threshold, target[queue].angle_threshold))
		{
			if (Abs(get_ticks() - delayCount) > 50)
			{
				tuneErrM();
				tuneErrW();
				parseMotorValues();
				delayCount = get_ticks();
			}
			accelClamp();
			M1 = getMotor1();
			M2 = getMotor2();
			M3 = getMotor3();
		}
		else
		{
			M1 = 0;
			M2 = 0;
			M3 = 0;
			if (queue < sizeof(target) / sizeof(Checkpoint))
			{
				queue++;
			}
			if (queue != sizeof(target) / sizeof(Checkpoint))
			{
				lockTarget(target[queue].position.vector, target[queue].position.orientation, target[queue].velocity);
				_delay(2);
			}
		}
		motor_set_vel(MOTOR1, M1, CLOSE_LOOP);
		motor_set_vel(MOTOR2, M2, CLOSE_LOOP);
		motor_set_vel(MOTOR3, M3, CLOSE_LOOP);
	}
}

void _updateScreen()
{
	tft_clear();
	tft_prints(16, 0, "%d", get_ticks());
	tft_prints(0, 1, " Current: %d %d [%d]", _getX(), _getY(), _getAngle()/10);
	tft_prints(0, 2, "Target-[%d]: %d %d [%d]", queue, getLockedTarget().position.vector.x, getLockedTarget().position.vector.y, getLockedTarget().position.orientation);
	tft_prints(0, 4, "parse: %d [%d] %d", getLastM(), getLastBearing(), getLastW());
	tft_prints(0, 6, "motors: %d %d %d", M1, M2, M3);
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
	return -get_pos()->x;
}

int _getY()
{
	//return get_pos()->y;
	return -get_pos()->y;
}

int _getAngle()
{
	return get_angle();
}

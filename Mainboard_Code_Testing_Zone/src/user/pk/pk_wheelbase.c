#include "pk_wheelbase.h"
#include "approx_math.h"
#include "can_motor.h"
#include "ticks.h"
#include "pk_math.h"

Wheelbase motors;

int xComponent;
int yComponent;
int rotation;

int M;
int bearing;
int W;

void pk_wheelbase_init() {
	motors.M1.sent = 0;
	motors.M1.target = 0;
	motors.M2.sent = 0;
	motors.M2.target = 0;
	motors.M3.sent = 0;
	motors.M3.target = 0;
	motors.lastTick = 0;
}

void reset() {
	xComponent = 0;
	yComponent = 0;
	rotation = 0;
}

void setM(int _M) {
	M = _M;
}

void setBearing(int _Bearing) {
	bearing = _Bearing % 360;
}

void setW(int _W) {
	W = _W;
}

void addComponent() {
	double x = M * MAX_VEL * int_sin(bearing * 10) / 10000 / 100;
	double y = M * MAX_VEL* int_cos(bearing * 10) / 10000 / 100;
	xComponent = xComponent + x;
	yComponent = yComponent + y;
	rotation = rotation + W;
	M = 0;
	bearing = 0;
	W = 0;
}

void parseWheelbaseValues() {
	motors.M1.target = (-rotation - xComponent * 2) / 3;
	motors.M2.target = ((-rotation * 0.5774 + xComponent * 0.5774 - yComponent) / 1.732);
	motors.M3.target = -rotation - motors.M1.target - motors.M2.target;
}

void sendWheelbaseCommand() {
	if (Abs(get_full_ticks() - motors.lastTick) >= INTERVAL) {
		motors.lastTick = get_full_ticks();
		int M1_diff = motors.M1.target - motors.M1.sent;
		int M2_diff = motors.M2.target - motors.M2.sent;
		int M3_diff = motors.M3.target - motors.M3.sent;
		float clampFactor = max(3, Abs(M1_diff), Abs(M2_diff), Abs(M3_diff)) / ACCELERATION;
		if (clampFactor < 1) {
			clampFactor = 1;
		}
		motors.M1.sent = motors.M1.sent + M1_diff / clampFactor;
		motors.M2.sent = motors.M2.sent + M2_diff / clampFactor;
		motors.M3.sent = motors.M3.sent + M3_diff / clampFactor;
	}
	motor_set_vel(MOTOR1, motors.M1.sent, CLOSE_LOOP);
	motor_set_vel(MOTOR2, motors.M2.sent, CLOSE_LOOP);
	motor_set_vel(MOTOR3, motors.M3.sent, CLOSE_LOOP);
	//reset();
}

void wheelbaseLock() {
	motors.lastTick = get_full_ticks();
	motors.M1.sent = 0;
	motors.M2.sent = 0;
	motors.M3.sent = 0;
	motors.M1.target = 0;
	motors.M2.target = 0;
	motors.M3.target = 0;
	motor_set_vel(MOTOR1, 0, CLOSE_LOOP);
	motor_set_vel(MOTOR2, 0, CLOSE_LOOP);
	motor_set_vel(MOTOR3, 0, CLOSE_LOOP);
}

Wheelbase getWheelbaseValues() {
	return motors;
}

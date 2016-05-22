#include "pk_wheelbase.h"
#include "approx_math.h"
#include "can_motor.h"
#include "ticks.h"
#include "pk_math.h"

Wheelbase motors;

void pk_wheelbase_init() {
	motors.M1.sent = 0;
	motors.M1.target = 0;
	motors.M2.sent = 0;
	motors.M2.target = 0;
	motors.M3.sent = 0;
	motors.M3.target = 0;
	motors.lastTick = 0;
}

void parseWheelbaseValues(int M, int bearing, int W) {
	int xComponent = M * MAX_VEL * int_sin(bearing*10)/10000 / 100;
	int yComponent = M * MAX_VEL* int_cos(bearing*10)/10000 / 100;
	motors.M1.target = (-W - xComponent * 2) / 3;
	motors.M2.target = ((-W * 0.5774 + xComponent * 0.5774 - yComponent) / 1.732);
	motors.M3.target = -W - motors.M1.target - motors.M2.target;
}

void sendWheelbaseValues() {
	if (Abs(get_full_ticks() - motors.lastTick) >= INTERVAL) {
		motors.lastTick = get_full_ticks();
		int M1_diff = motors.M1.target - motors.M1.sent;
		int M2_diff = motors.M2.target - motors.M2.sent;
		int M3_diff = motors.M3.target - motors.M3.sent;
		int clampFactor = max(3, Abs(M1_diff), Abs(M2_diff), Abs(M3_diff)) / ACCELERATION;
		if (clampFactor < 1) {
			clampFactor = 1;
		}
		motors.M1.sent = motors.M1.sent + M1_diff / clampFactor;
		motors.M2.sent = motors.M1.sent + M2_diff / clampFactor;
		motors.M3.sent = motors.M1.sent + M3_diff / clampFactor;
	}
	motor_set_vel(MOTOR1, motors.M1.sent, CLOSE_LOOP);
	motor_set_vel(MOTOR2, motors.M2.sent, CLOSE_LOOP);
	motor_set_vel(MOTOR3, motors.M3.sent, CLOSE_LOOP);
}

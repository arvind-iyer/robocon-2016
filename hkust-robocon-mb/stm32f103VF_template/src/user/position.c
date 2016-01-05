#include "position.h"
#include "can_motor.h"

s32 TARGET_X = 0;
s32 TARGET_Y = 0;
s32 TARGET_TICKS = 0;

s32 ACC_X_ERROR = 0;
s32 ACC_Y_ERROR = 0;

s32 ROBOT_POS_X = 0;
s32 ROBOT_POS_Y = 0;

s32 TARGET_DIRECTION = 0;
s32 LINE_DIRECTION = 0;
s32 LINE_DISTANCE = 0;

int ROBOT_MOVING = 0;

void setRobotVelocity(int angle, int velocity) {
	motor_set_vel(MOTOR1, int_sin(angle) / (float)10000 * velocity * -1, CLOSE_LOOP);
	motor_set_vel(MOTOR2, int_sin(angle + 1200) / (float)10000 * velocity * -1, CLOSE_LOOP);
	motor_set_vel(MOTOR3, int_sin(angle + 2400) / (float)10000 * velocity * -1, CLOSE_LOOP);
}

void lockAllMotors() {
	motor_lock(MOTOR1);
	motor_lock(MOTOR2);
	motor_lock(MOTOR3);
}

int calculateAngleFromPos(int x, int y) {
	int angle = int_arc_tan2(y - get_Y(), x - get_X());
	if (angle < 0) angle += 3600;
	angle = (900 - angle);
	return (angle < 0 ? 3600 + angle : angle);
}

void setTargetLocation(int x, int y) {
	TARGET_X = x;
	TARGET_Y = y;
	
	LINE_DISTANCE = Sqrt(Sqr(get_Y() - y) + Sqr(get_X() - x));
	TARGET_DIRECTION = LINE_DIRECTION = calculateAngleFromPos(TARGET_X, TARGET_Y);
	TARGET_TICKS = get_full_ticks();
	ROBOT_MOVING = 1;
}

s32 getXError() {
	s32 xVelocity = (int_cos(LINE_DIRECTION) / (s32) 10000) * ROBOT_VELOCITY;
	return (xVelocity * ((get_full_ticks() - TARGET_TICKS) / (s16) 1000))  - (s32) get_X();
}

s32 getYError() {
	s32 yVelocity = (int_sin(LINE_DIRECTION) / (s32) 10000) * ROBOT_VELOCITY;
	return (yVelocity * ((get_full_ticks() - TARGET_TICKS) / (s16) 1000))  - (s32) get_Y();
}

void updateRobotPosition() {
	static s32 X_ERROR = 0;
	static s32 Y_ERROR = 0;
	static s32 PREV_X_ERROR = 0;
	static s32 PREV_X_2_ERROR = 0;
	static s32 PREV_Y_ERROR = 0;
	static s32 PREV_Y_2_ERROR = 0;
	
	X_ERROR = getXError();
	Y_ERROR = getYError();
	
	s32 pidX = (KP * (X_ERROR - PREV_X_ERROR)) + (KI * X_ERROR) + (KD * (X_ERROR + PREV_X_2_ERROR - 2 * PREV_X_ERROR));
	s32 pidY = (KP * (Y_ERROR - PREV_Y_ERROR)) + (KI * Y_ERROR) + (KD * (Y_ERROR + PREV_Y_2_ERROR - 2 * PREV_Y_ERROR));
	
	PREV_X_2_ERROR = PREV_X_ERROR;
	PREV_X_ERROR = X_ERROR;
	
	PREV_Y_2_ERROR = PREV_Y_ERROR;
	PREV_Y_ERROR = Y_ERROR;
	
	ROBOT_POS_X = get_X() + (pidX / 10);
	ROBOT_POS_Y = get_Y() + (pidY / 10);
}

void sendDebugInfo() {
	uart_tx(COM2, "DISTANCE|%d|d\n", Sqrt(Sqr(ROBOT_POS_Y - TARGET_Y) + Sqr(ROBOT_POS_X - TARGET_X)), calculateAngleFromPos(ROBOT_POS_X, ROBOT_POS_Y));
}

void pursueTarget() {
	if (ROBOT_MOVING == 1) {
		updateRobotPosition();
		sendDebugInfo();
		int pidAngle = calculateAngleFromPos(ROBOT_POS_X, ROBOT_POS_Y);
		if (Sqrt(Sqr(get_Y() - TARGET_Y) + Sqr(get_X() - TARGET_X)) <= 400) {
			setRobotVelocity(TARGET_DIRECTION, 0);
		} else {
			setRobotVelocity((LINE_DIRECTION + get_angle() + pidAngle) % 3600, MAX_VELOCITY);
		}
	}
}
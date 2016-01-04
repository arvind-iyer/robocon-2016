#include "position.h"
#include "can_motor.h"
#include "approx_math.h"
#include "gyro.h"
#include "ticks.h"

int TARGET_X = 0;
int TARGET_Y = 0;
int TARGET_TICKS = 0;

int ACC_X_ERROR = 0;
int ACC_Y_ERROR = 0;

int ROBOT_POS_X = 0;
int ROBOT_POS_Y = 0;

int TARGET_DIRECTION = 0;
int LINE_DIRECTION = 0;
int LINE_DISTANCE = 0;

int ROBOT_MOVING = 0;

void setRobotVelocity(int angle, int velocity) {
	motor_set_vel(MOTOR1, int_sin(angle * 10) / (uint32_t)10000 * velocity * -1, CLOSE_LOOP);
	motor_set_vel(MOTOR2, int_sin((angle + 120) * 10) / (uint32_t)10000 * velocity * -1, CLOSE_LOOP);
	motor_set_vel(MOTOR3, int_sin((angle + 240) * 10) / (uint32_t)10000 * velocity * -1, CLOSE_LOOP);
}

void lockAllMotors() {
	motor_lock(MOTOR1);
	motor_lock(MOTOR2);
	motor_lock(MOTOR3);
}

void sendDebugInfo() {
	uart_tx(COM2, "DISTANCE|%d\n", Sqrt(Sqr(ROBOT_POS_Y - TARGET_Y) + Sqr(ROBOT_POS_X - TARGET_X)));
}

int calculateAngleFromPos(int x, int y) {
	int angle = int_arc_tan2(TARGET_Y - ROBOT_POS_Y, TARGET_X - ROBOT_POS_X);
	if (angle < 0) angle += 360;
	angle = 90 - angle;
	return angle < 0 ? 360 + angle : angle;
}

void setTargetLocation(int x, int y) {
	TARGET_X = x;
	TARGET_Y = y;
	
	LINE_DISTANCE = Sqrt(Sqr(get_Y() - y) + Sqr(get_X() - x));
	LINE_DIRECTION = calculateAngleFromPos(x, y);
	TARGET_TICKS = get_seconds();
	ROBOT_MOVING = 1;
}

uint32_t getXError() {
	uint32_t xVelocity = (int_cos(LINE_DIRECTION * 10) / (uint32_t) 10000) * ROBOT_VELOCITY;
	return (xVelocity * (get_seconds() - TARGET_TICKS))  - get_X();
}

uint32_t getYError() {
	uint32_t yVelocity = (int_sin(LINE_DIRECTION * 10) / (uint32_t) 10000) * ROBOT_VELOCITY;
	return (yVelocity * (get_seconds() - TARGET_TICKS))  - get_Y();
}

void updateRobotPosition() {
	static uint32_t X_ERROR = 0;
	static uint32_t Y_ERROR = 0;
	static uint32_t PREV_X_ERROR = 0;
	static uint32_t PREV_X_2_ERROR = 0;
	static uint32_t PREV_Y_ERROR = 0;
	static uint32_t PREV_Y_2_ERROR = 0;
	
	X_ERROR = getXError();
	Y_ERROR = getYError();
	
	uint32_t pidX = (X_KP * (X_ERROR - PREV_X_ERROR)) + (X_KI * X_ERROR) + (X_KD * (X_ERROR + PREV_X_2_ERROR - 2 * PREV_X_ERROR));
	uint32_t pidY = (Y_KP * (Y_ERROR - PREV_Y_ERROR)) + (Y_KI * Y_ERROR) + (Y_KD * (Y_ERROR + PREV_Y_2_ERROR - 2 * PREV_Y_ERROR));
	
	PREV_X_2_ERROR = PREV_X_ERROR;
	PREV_X_ERROR = X_ERROR;
	
	PREV_Y_2_ERROR = PREV_Y_ERROR;
	PREV_Y_ERROR = Y_ERROR;
	
	uart_tx(COM2, "PID|%d|%d|%d|%d|%d\n", pidX, pidY, X_ERROR, Y_ERROR, get_seconds() - TARGET_TICKS);
	
	ROBOT_POS_X = get_X() + pidX;
	ROBOT_POS_Y = get_Y() + pidY;
}

void pursueTarget() {
	sendDebugInfo();
	updateRobotPosition();
	TARGET_DIRECTION = calculateAngleFromPos(TARGET_X, TARGET_Y);
	if (ROBOT_MOVING == 1) {
		if (Sqrt(Sqr(ROBOT_POS_Y - TARGET_Y) + Sqr(ROBOT_POS_X - TARGET_X)) <= 380) {
			lockAllMotors();
		} else {
			setRobotVelocity(TARGET_DIRECTION, MAX_VELOCITY);
		}
	}
}
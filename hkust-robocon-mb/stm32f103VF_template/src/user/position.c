// PID-controlled Positioning System
// Created by: Kenta Iwasaki & Kristian Suhartono
// Jan 06, 2016

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

void setRobotVelocity(int angle, int velocity, int flag) {
	motor_set_vel(MOTOR1, int_sin(angle) / (float)10000 * velocity * -1, flag);
	motor_set_vel(MOTOR2, int_sin(angle + 1200) / (float)10000 * velocity * -1, flag);
	motor_set_vel(MOTOR3, int_sin(angle + 2400) / (float)10000 * velocity * -1, flag);
}

void lockAllMotors() {
	motor_lock(MOTOR1);
	motor_lock(MOTOR2);
	motor_lock(MOTOR3);
}

int calculateAngleFromPos(int x, int y) {
	int angle = int_arc_tan2(y - get_pos()->y, x - get_pos()->x);
	return angle;
}

int calculateBearingFromPos(int x, int y) {
	int angle = (900 - (int_arc_tan2(y - get_pos()->y, x - get_pos()->x) * 10)) - get_pos()->angle;
	while (angle < 0) angle += 3600;
	angle = angle % 3600;
	return angle;
}

void setTargetLocation(int x, int y) {
	TARGET_X = x;
	TARGET_Y = y;
	
	LINE_DISTANCE = Sqrt(Sqr(get_pos()->y - y) + Sqr(get_pos()->x - x));
	TARGET_DIRECTION = LINE_DIRECTION = calculateBearingFromPos(TARGET_X, TARGET_Y);
	TARGET_TICKS = get_full_ticks();
	ROBOT_MOVING = 1;
}

s32 getXError() {
	s32 xVelocity = (int_cos(LINE_DIRECTION) / (s32) 10000) * ROBOT_VELOCITY;
	return (xVelocity * ((get_full_ticks() - TARGET_TICKS) / (s16) 1000))  - (s32) get_pos()->x;
}

s32 getYError() {
	s32 yVelocity = (int_sin(LINE_DIRECTION) / (s32) 10000) * ROBOT_VELOCITY;
	return (yVelocity * ((get_full_ticks() - TARGET_TICKS) / (s16) 1000))  - (s32) get_pos()->y;
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
	
	ROBOT_POS_X = get_pos()->x + (pidX / 10);
	ROBOT_POS_Y = get_pos()->y + (pidY / 10);
}

void sendDebugInfo() {
	uart_tx(COM2, "DISTANCE|%d|%d|%d|%d|%d|%d|%f\n", get_pos()->x, get_pos()->y, ROBOT_POS_X, ROBOT_POS_Y, get_pos()->angle / 10, TARGET_DIRECTION / 10, (Sqrt(Sqr(get_pos()->y - TARGET_Y) + Sqr(get_pos()->x - TARGET_X))) / (float) (LINE_DISTANCE));
}

void pursueTarget() {
	static uint32_t lastDst = 0;
	
	LINE_DIRECTION = calculateBearingFromPos(TARGET_X, TARGET_Y);
	TARGET_DIRECTION = (LINE_DIRECTION - calculateAngleFromPos(ROBOT_POS_X, ROBOT_POS_Y));
	while (TARGET_DIRECTION < 0) TARGET_DIRECTION += 3600;
	TARGET_DIRECTION = TARGET_DIRECTION % 3600;
	
	if (ROBOT_MOVING == 1) {
		updateRobotPosition();
		
		uint32_t dst = Sqrt(Sqr(get_pos()->y - TARGET_Y) + Sqr(get_pos()->x - TARGET_X));
		if (lastDst == 0) lastDst = dst;
		
		if (lastDst < dst) {
			setRobotVelocity(TARGET_DIRECTION, 0, OPEN_LOOP);
			lastDst = 999999;
			_delay_ms(100);
		} else {
			if (dst <= 500) {
				lockAllMotors();
				setRobotVelocity(TARGET_DIRECTION, 0, OPEN_LOOP);
			} else {
				//setRobotVelocity(TARGET_DIRECTION, (dst / (float) LINE_DISTANCE) * MAX_VELOCITY, CLOSE_LOOP);
				setRobotVelocity(TARGET_DIRECTION, MAX_VELOCITY, CLOSE_LOOP);
			}
		}
		
		lastDst = dst;
		sendDebugInfo();
	}
}
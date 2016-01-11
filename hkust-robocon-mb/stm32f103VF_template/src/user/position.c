// PID-controlled Positioning System
// Created by: Kenta Iwasaki & Kristian Suhartono
// Jan 06, 2016
#include "position.h"
#include "can_motor.h"
#include <stdio.h>
#include <stdlib.h>

s32 TARGET_X = 0;
s32 TARGET_Y = 0;
s32 TARGET_TICKS = 0;

s32 TARGET_DIRECTION = 0;
s32 LINE_DISTANCE = 0;
s32 LINE_DIRECTION = 0;

PID velXPid;
PID velYPid;
PID velWPid;

int ROBOT_MOVING = 0;
int currentTargetIndex = 0;
int stopTime = 0;

TargetQueue targetQueue;
Target target;

void initTargetQueue(uint32_t initialSize) {
	targetQueue = (TargetQueue) {.array = (Target *)malloc(initialSize * sizeof(Target)), .used = 0, .size = initialSize};
}

void queueTarget(Target element) {
  if (targetQueue.used == targetQueue.size) {
    targetQueue.size *= 2;
    targetQueue.array = (Target *)realloc(targetQueue.array, targetQueue.size * sizeof(Target));
  }
  targetQueue.array[targetQueue.used++] = element;
}

void freeQueue() {
  free(targetQueue.array);
  targetQueue.array = NULL;
  targetQueue.used = targetQueue.size = 0;
}

void setRobotVelocity(s32 vel, s32 angle, s32 vw, int flag) {
	int32_t velocities[3] = { int_sin(angle) * vel / 10000, int_sin(angle + 1200) * vel / 10000,
		int_sin(angle + 2400) * vel / 10000};

	for (int i = 0; i < 3; i++) {
		velocities[i] += vw;
		velocities[i] *= -1;

		if (velocities[i] > MAX_MAGNITUDE) velocities[i] = MAX_MAGNITUDE;
		if (velocities[i] < -MAX_MAGNITUDE) velocities[i] = -MAX_MAGNITUDE;
	}
	
	motor_set_vel(MOTOR1, velocities[0], flag);
	motor_set_vel(MOTOR2, velocities[1], flag);
	motor_set_vel(MOTOR3, velocities[2], flag);
}

void lockAllMotors() {
	motor_lock(MOTOR1);
	motor_lock(MOTOR2);
	motor_lock(MOTOR3);
}

int calculateAngleFromPos(int x, int y) {
	int angle = int_arc_tan2(y - get_pos()->y, x - get_pos()->x);
	return angle * 10;
}

int calculateBearingFromPos(int x, int y) {
	int angle = (900 - (int_arc_tan2(y - get_pos()->y, x - get_pos()->x) * 10)) - get_pos()->angle;
	while (angle < 0) angle += 3600;
	angle = angle % 3600;
	return angle;
}

void setTargetLocation(int x, int y, int angle) {
	TARGET_X = x;
	TARGET_Y = y;
	LINE_DIRECTION = angle;
	
	LINE_DISTANCE = Sqrt(Sqr(get_pos()->y - y) + Sqr(get_pos()->x - x));
	TARGET_TICKS = get_full_ticks();
	ROBOT_MOVING = 1;
	
	// Initialize PID. // 87
	velXPid = (PID) {.Kp = 85, .Ki = 0, .Kd = 1500, .threshold = 20, .scale = 100,
	.error = TARGET_X - get_pos()->x, .min = -Abs(30000 - get_pos()->x), .max = Abs(30000 - get_pos()->x)};
	
	velYPid = (PID) {.Kp = 85, .Ki = 0, .Kd = 1500, .threshold = 20, .scale = 100,
	.error = TARGET_Y - get_pos()->y, .min = -Abs(30000 - get_pos()->y), .max = Abs(30000 - get_pos()->y)};

	s32 dw = LINE_DIRECTION - get_pos()->angle;
	while (dw < -1800) dw += 3600;
	while (dw > 1800) dw -= 3600;
	
	velWPid = (PID) {.Kp = 50, .Ki = 0, .Kd = 1000, .threshold = 40, .scale = 100,
	.error = dw, .min = -3600, .max = 3600};
}

void dequeueTarget() {
	target = targetQueue.array[currentTargetIndex];
	setTargetLocation(target.x, target.y, target.angle);
	currentTargetIndex++;
	
	if (currentTargetIndex >= targetQueue.used) currentTargetIndex = 0;
}

void updateRobotPosition() {
	updatePid(&velXPid, TARGET_X - get_pos()->x);
	updatePid(&velYPid, TARGET_Y - get_pos()->y);
	
	s32 dw = LINE_DIRECTION - get_pos()->angle;
	while (dw < -1800) dw += 3600;
	while (dw > 1800) dw -= 3600;
	
	updatePid(&velWPid, dw);
}

//void sendDebugInfo() {
//	uart_tx(COM2, "DISTANCE|%d|%d|%d\n", velXPid.output, velYPid.output, velWPid.output);
//}

void pursueTarget() {
	TARGET_DIRECTION = calculateBearingFromPos(TARGET_X, TARGET_Y);
	while (TARGET_DIRECTION < 0) TARGET_DIRECTION += 3600;
	TARGET_DIRECTION = TARGET_DIRECTION % 3600;
	
	if (ROBOT_MOVING == 1) {
		updateRobotPosition();
		
		if (Abs(velXPid.output) <= velXPid.threshold && Abs(velYPid.output) <= velYPid.threshold && Abs(velWPid.output) <= velWPid.threshold) {
				lockAllMotors();
				_delay_ms(100);
				setRobotVelocity(0, 0, 0, OPEN_LOOP);
				
			// Move to next queue.
			if (get_full_ticks() - stopTime >= 500) {
				dequeueTarget(&targetQueue);
				stopTime = get_full_ticks();
			}
		} else {
			stopTime = get_full_ticks();
			
			s32 distance = Sqrt(Sqr(velXPid.output) + Sqr(velYPid.output));
			s32 magnitude = distance * (MAX_MAGNITUDE) / LINE_DISTANCE;	
			
			int32_t curveAngle = (900 * (magnitude * 100 / 1000) / 100);
			
			if (curveAngle <= 40) target.curveFactor = NO_CURVE;
			s32 translateAngle = TARGET_DIRECTION + (target.curveFactor == NO_CURVE ? 0 : curveAngle);
			
			while (translateAngle < 0) translateAngle += 3600;
			translateAngle = translateAngle % 3600;
			
			s32 angleMag = velWPid.output;
			angleMag = angleMag * (MAX_MAGNITUDE - magnitude) / 3600;
			
			uart_tx(COM2, "DISTANCE|%d|%d|%d|%d\n", magnitude, TARGET_DIRECTION, curveAngle, translateAngle);
			
			setRobotVelocity(magnitude, translateAngle, angleMag, CLOSE_LOOP);
			
		}
	}
}
// PID-controlled Positioning System
// Created by: Kenta Iwasaki & Kristian Suhartono
// Jan 06, 2016
#include "position.h"
#include "can_motor.h"
#include <stdio.h>
#include <stdlib.h>

float START_X = 0;
float START_Y = 0;
float TARGET_X = 0;
float TARGET_Y = 0;
s32 TARGET_TICKS = 0;

int TARGET_DIRECTION = 0;
float LINE_DISTANCE = 0;
float LINE_DIRECTION = 0;

#define MAX_CURVE_POINTS 5

int ROBOT_MOVING = 0;
int currentTargetIndex = 0;
int stopTime = 0;

TargetQueue targetQueue;
Target target;
Target curvePoint;
Target curve[MAX_CURVE_POINTS - 2];

int curveIndex = 0;

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

float lastX = 0;
float lastY = 0;
float lastAngle = 0;
int lastRecorded = 0;
float err = 1;

float velocities[3];

float getPathCorrectionAngle() {
	float diffTargetY = TARGET_Y - get_pos()->y;
	float diffTargetX = TARGET_X - get_pos()->x;
	float angleToTarget = int_arc_tan2(diffTargetY, diffTargetX) * 10;
	float targetDst = Sqrt(Sqr(diffTargetY)
			+ Sqr(diffTargetX));

	float diffLineX = TARGET_X - START_X;
	float diffLineY = TARGET_Y - START_Y;
	float u = ((get_pos()->x - START_X) * diffLineX + (get_pos()->y - START_Y) * diffLineY)
			/ ((float) diffLineX * diffLineX + diffLineY * diffLineY);
	float linePointX = START_X + u * diffLineX;
	float linePointY = START_Y + u * diffLineY;

	float diffPointX = linePointX - get_pos()->x;
	float diffPointY = linePointY - get_pos()->y;
	float dstToPoint = (float) Sqrt(Sqr(diffPointX)
			+ Sqr(diffPointY));

	float angleToLine = int_arc_tan2(diffPointY, diffPointX) * 10;

	return (900 - int_arc_tan2(targetDst * int_sin(angleToTarget)
					+ dstToPoint * int_sin(angleToLine),
					targetDst * int_cos(angleToTarget) + dstToPoint
							* int_cos(angleToLine)) * 10) - get_pos()->angle;

}

// Magnitude: from 0 to 100.
// Bearing: Translation Angle.
// W: Angular Velocity
void setRobotVelocity(float magnitude, float translationAngle, float angularVelocity) {
	if (magnitude > MAX_MAGNITUDE) {
		magnitude = MAX_MAGNITUDE;
	}
	float X = magnitude * int_sin(translationAngle) * MAX_VELOCITY / 100 / 10000;
	float Y = magnitude * int_cos(translationAngle) * MAX_VELOCITY / 100 / 10000;

	velocities[0] = (-angularVelocity - X * 2) / 3;
	velocities[1] = (-angularVelocity * Sqrt(3) / 3 + X * Sqrt(3) / 3 - Y) / Sqrt(3);
	velocities[2] = -angularVelocity - velocities[0] - velocities[1];
	if ((magnitude != 0 || angularVelocity != 0) && lastX == get_pos()->x
			&& lastY == get_pos()->y && lastAngle == get_pos()->angle) {
		if (velocities[0] * (err + 0.03f) <= MAX_VELOCITY
				&& velocities[1] * (err + 0.03f) <= MAX_VELOCITY
				&& velocities[2] * (err + 0.03f) <= MAX_VELOCITY) {
			err += 0.2f;
		} else {
			// Motor is dead.
		}
	} else {
		err = err / 2 + 0.5f;
		if (err < 1) {
			err = 1;
		}
	}
	for (int i = 0; i < 3; i++)
		velocities[i] *= err;
	//motor control
	motor_set_vel(MOTOR1, velocities[0], CLOSE_LOOP);
	motor_set_vel(MOTOR2, velocities[1], CLOSE_LOOP);
	motor_set_vel(MOTOR3, velocities[2], CLOSE_LOOP);

	lastX = get_pos()->x;
	lastY = get_pos()->y;
	lastAngle = get_pos()->angle;
	lastRecorded = get_full_ticks();
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
	START_X = get_pos()->x;
	START_Y = get_pos()->y;
	
	TARGET_X = x;
	TARGET_Y = y;
	LINE_DIRECTION = angle;
	
	LINE_DISTANCE = Sqrt(Sqr(get_pos()->y - y) + Sqr(get_pos()->x - x));
	TARGET_TICKS = get_full_ticks();
	ROBOT_MOVING = 1;
}

void approximateCurve(int32_t x, int32_t y,
		Target target, int32_t bendFactor) {
	Target center = { .x = target.x - x, .y = target.y - y };
	int32_t radius = Sqrt(Sqr(center.x) + Sqr(center.y)) / 2;
	int32_t angle = int_arc_tan2(center.y, center.x) * 10 + 900 + bendFactor;
	center.x = center.x / 2 + (radius * int_cos(angle) / 10000);
	center.y = center.y / 2 + (radius * int_sin(angle) / 10000);

	for (int32_t i = 0; i <= MAX_CURVE_POINTS - 2; ++i) {
		int32_t t = (i + 1) * 100 / MAX_CURVE_POINTS;
		int32_t a = Sqr(100 - t);
		int32_t b = 2 * t * (100 - t);
		int32_t c = Sqr(t);
		curve[i] = (Target) { .x = (a * x + b * center.x + c * target.x) / 10000,
						.y = (a * y + b * center.y + c * target.y) / 10000 };
	}
}
		
void dequeueTarget() {
	target = targetQueue.array[currentTargetIndex];
	setTargetLocation(target.x, target.y, target.angle);
	if (target.curveFactor != NO_CURVE) {
		curveIndex = 0;
		approximateCurve(get_pos()->x, get_pos()->y, target, target.curveFactor);
		curvePoint = curve[curveIndex];
	}
	currentTargetIndex++;
	
	if (currentTargetIndex >= targetQueue.used) currentTargetIndex = 0;
}

void updateRobotPosition() {
	// Update PID's here.
}

void updateTargetDirection() {
	if (target.curveFactor != NO_CURVE) {
		if (Sqrt(Sqr(curvePoint.y - get_pos()->y) + Sqr(curvePoint.x - get_pos()->x)) <= 20) {
			curveIndex++;
			if (curveIndex >= MAX_CURVE_POINTS)
				target.curveFactor = NO_CURVE;
			else
				curvePoint = curve[curveIndex];
		}
		TARGET_DIRECTION = calculateBearingFromPos(curvePoint.x, curvePoint.y);
	} else {
		TARGET_DIRECTION = getPathCorrectionAngle();
	}
	
	while (TARGET_DIRECTION < 0) TARGET_DIRECTION += 3600;
	TARGET_DIRECTION = TARGET_DIRECTION % 3600;
}
	
float lastCompletionPercentage = -1;

void pursueTarget() {
	if (ROBOT_MOVING == 1) {
		updateRobotPosition();
		updateTargetDirection();
		
		float distance = Sqrt(Sqr(TARGET_Y - get_pos()->y) + Sqr(TARGET_X - get_pos()->x));
		float dw = LINE_DIRECTION - get_pos()->angle;
		while (dw < -1800) dw += 3600;
		while (dw > 1800) dw -= 3600;
		
		if (distance <= 10 && dw <= 20) {
			setRobotVelocity(0, 0, 0);
			// Move to next queue.
			//if (get_full_ticks() - stopTime >= 100) {
				//dequeueTarget();
				//stopTime = get_full_ticks();
			//}
		} else {
			stopTime = get_full_ticks();
			float magnitude = distance / (float) 10;
			
			float completedPathPercentage = 100 - (distance / (float) LINE_DISTANCE) * 100;
			if (completedPathPercentage < 0) completedPathPercentage = 0;
			else if (completedPathPercentage > 100) completedPathPercentage = 100;
			
			// Acceleration [from 0 to 15%]
			// Vi = 10, Vf = 100
			//if (completedPathPercentage <= 15 && lastCompletionPercentage != -1 &&
			//	lastCompletionPercentage - completedPathPercentage >= 0) magnitude = 10 + 90 * (completedPathPercentage / 15);
			
			float angleMag = (dw / 3600) * 150;
			
			uart_tx(COM2, "DISTANCE|%f|%d|%f\n", magnitude, TARGET_DIRECTION, angleMag);
			setRobotVelocity(magnitude, TARGET_DIRECTION, angleMag);
			
			lastCompletionPercentage = completedPathPercentage;
		}
	}
}
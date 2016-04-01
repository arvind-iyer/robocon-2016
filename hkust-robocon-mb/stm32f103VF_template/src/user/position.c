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

#define MAX_CURVE_POINTS 10

int ROBOT_MOVING = 0;
int currentTargetIndex = 0;
int stopTime = 0;

TargetQueue targetQueue;
Target target;
Target curvePoint;
Target nextCurvePoint;
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

float getPathCorrectionAngle(int startX, int startY, int targetX, int targetY) {
	float diffTargetY = targetY - get_pos()->y;
	float diffTargetX = targetX - get_pos()->x;
	float angleToTarget = int_arc_tan2(diffTargetY, diffTargetX) * 10;
	float targetDst = Sqrt(Sqr(diffTargetY)
			+ Sqr(diffTargetX));

	float diffLineX = targetX - startX;
	float diffLineY = targetY - startY;
	float u = ((get_pos()->x - startX) * diffLineX + (get_pos()->y - startY) * diffLineY)
			/ ((float) diffLineX * diffLineX + diffLineY * diffLineY);
	float linePointX = startX + u * diffLineX;
	float linePointY = startY + u * diffLineY;

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
		if (velocities[0] * (err + 0.5f) <= MAX_VELOCITY
				&& velocities[1] * (err + 0.5f) <= MAX_VELOCITY
				&& velocities[2] * (err + 0.5f) <= MAX_VELOCITY) {
			err *= 1.2f;
		} else {
			// Motor is dead.
		}
	} else {
		err *= 0.8f;
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

void approximateCurve(Target pts[MAX_CURVE_POINTS - 2], float x, float y,
		Target target, float bendFactor) {
	Target center = { .x = target.x - x, .y = target.y - y };
	float radius = Sqrt(Sqr(center.x) + Sqr(center.y)) / 2;
	float angle = int_arc_tan2(center.y, center.x) * 10 + 900 + bendFactor;
	center.x = center.x / 2 + (radius * int_cos(angle) / 10000);
	center.y = center.y / 2 + (radius * int_sin(angle) / 10000);

	for (unsigned int i = 0; i <= MAX_CURVE_POINTS - 2; ++i) {
		float t = (i + 1) / (float) MAX_CURVE_POINTS;
		pts[i] = (Target ) { .x = ((1 - t) * (1 - t) * x
						+ 2 * (1 - t) * t * center.x + t * t * target.x), .y =
						(1 - t) * (1 - t) * y + 2 * (1 - t) * t * center.y
								+ t * t * target.y };
	}
}
		
int lastCurveDistance = 0;
		
void dequeueTarget() {
	target = targetQueue.array[currentTargetIndex];
	setTargetLocation(target.x, target.y, target.angle);
	if (target.curveFactor != NO_CURVE) {
		curveIndex = 0;
		approximateCurve(curve, get_pos()->x, get_pos()->y, target, target.curveFactor);
		curvePoint = (Target) {.x = START_X, .y = START_Y};
		nextCurvePoint = curve[curveIndex];
		
		lastCurveDistance = Sqrt(Sqr(nextCurvePoint.y - curvePoint.y) + Sqr(nextCurvePoint.x - curvePoint.x));
	}
	currentTargetIndex++;
	
	if (currentTargetIndex >= targetQueue.used) currentTargetIndex = 0;
}

void updateRobotPosition() {
	// Update PID's here.
}

void updateTargetDirection() {
	if (target.curveFactor != NO_CURVE) {
		int distanceToCurve = Sqrt(Sqr(nextCurvePoint.y - get_pos()->y) + Sqr(nextCurvePoint.x - get_pos()->x));
		if (distanceToCurve <= 300 && lastCurveDistance - distanceToCurve < 0) {
			curveIndex++;
			uart_tx(COM2, "DISTANCE|%d\n", curveIndex);
			if (curveIndex >= MAX_CURVE_POINTS - 2) {
				target.curveFactor = NO_CURVE;
			} else {
				curvePoint = curve[curveIndex - 1];
				nextCurvePoint = curve[curveIndex];
				lastCurveDistance = Sqrt(Sqr(nextCurvePoint.y - curvePoint.y) + Sqr(nextCurvePoint.x - curvePoint.x));
			}
		}
		TARGET_DIRECTION = getPathCorrectionAngle(curvePoint.x, curvePoint.y, nextCurvePoint.x, nextCurvePoint.y);
		lastCurveDistance = distanceToCurve;
	} else {
		TARGET_DIRECTION = getPathCorrectionAngle(START_X, START_Y, TARGET_X, TARGET_Y);
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
			if (get_full_ticks() - stopTime >= 50) {
				dequeueTarget();
				stopTime = get_full_ticks();
			}
		} else {
			stopTime = get_full_ticks();
			float magnitude = distance / (float) 20;
			
			float completedPathPercentage = 100 - (distance / (float) LINE_DISTANCE) * 100;
			if (completedPathPercentage < 0) completedPathPercentage = 0;
			else if (completedPathPercentage > 100) completedPathPercentage = 100;
			
			// Acceleration [from 0 to 15%]
			// Vi = 10, Vf = 100
			//if (completedPathPercentage <= 15 && lastCompletionPercentage != -1 &&
			//	lastCompletionPercentage - completedPathPercentage <= 0) magnitude = 10 + 90 * (completedPathPercentage / 15);
			
			float angleMag = (dw / 3600) * 150;
			
			setRobotVelocity(magnitude, TARGET_DIRECTION, angleMag);
			
			lastCompletionPercentage = completedPathPercentage;
		}
	}
}
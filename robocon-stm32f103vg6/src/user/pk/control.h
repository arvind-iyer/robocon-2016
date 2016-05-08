#ifndef __control_h
#define __control_h

#include "pk_movement.h"
#include "gyro.h"
#include "approx_math.h"
#include "hybrid_gpio.h"
#include <stdbool.h>
#include "pk_manual.h"
#include "pk_movement.h"

typedef struct {
	POSITION position;
	float distanceThreshold;
	float bearingThreshold;
	int vel;
	} Path;

	typedef struct {
		POSITION position;
		POSITION start;
		int pathLength;
	} Robot;
	
	void calculatePIDMotorValues(int vel, int bearing, int w);
	void robotUpdate();
	int MAX(int a, int b);
	int MIN(int a, int b);
	int calculateDistance(POSITION origin, POSITION target);
	int calculatePathVelocity (Path path, Robot robot);
	int calculatePathBearing (Path path, Robot robot);
	int calculatePathAngularVelocity(Path path, Robot robot);
	int binomial(int n, int k);
	int getAngleDifference (int origin, int target);
	void updateQueue();
	void robotInit();
	void dequeue(int size);
	
	extern Motors motor;
	extern bool allowArm;

#endif
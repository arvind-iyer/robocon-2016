#ifndef __control_h
#define __control_h

#include "pk_movement.h"
#include "gyro.h"
#include "approx_math.h"
#include "hybrid_gpio.h"
#include <stdbool.h>
#include "pk_manual.h"
#include "pk_movement.h"
#include "pk_brushless.h"
#include "hybrid_pneumatic.h"

typedef struct {
	POSITION position;
	int distanceThreshold;
	int bearingThreshold;
	int vel;
	int brushlessSpeed;
	int waitTime;
	} Path;

	typedef struct {
		POSITION position;
		POSITION start;
		int pathLength;
	} Robot;
	
	void calculatePIDMotorValues(int vel, int bearing, int w);
	void robotUpdate(void);
	//int MAX(int a, int b);
	float MAX(float a, float b);
	//int MIN(int a, int b);
	float MIN(float a ,float b);
	int calculateDistance(POSITION origin, POSITION target);
	int calculatePathVelocity (Path path, Robot robot);
	int calculatePathBearing (Path path, Robot robot);
	int calculatePathAngularVelocity(Path path, Robot robot);
	int binomial(int n, int k);
	int getAngleDifference (int origin, int target);
	void updateQueue(void);
	void robotInit(void);
	void dequeue(int _size);
	void queueTargetPoint(int x, int y, int bearing, float thres, float bearThres, int brushlessSpeed, int waitTime);
	int getSize(void);
	void riverBlow();
	
	extern Motors motor;
	extern Robot robot;
	extern bool allowArm;
	extern int dispM;

#endif

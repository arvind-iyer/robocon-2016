#include "control.h"

#define PI 3.14159265359
#define MAX_MOTOR_PWM 160

Robot robot;
Path queue [40];
int size= 0;

void calculatePIDMotorValues(int vel, int bearing, int w){
	int x = (vel/100) * int_sin(bearing * PI / 180) * MAX_MOTOR_PWM;
	int y = (vel/100) * int_cos(bearing * PI / 180) * MAX_MOTOR_PWM;
	motor.M1 = (-w -x *2) / 3;
	motor.M2 = (-w * Sqrt(1/3) + x * Sqrt(1/3) - y) / Sqrt(3);
	motor.M3 = -w - motor.M1 - motor.M2;
}

void robotInit() {
	robot.position.x = get_pos()->x;
	robot.position.y = get_pos()->y;
	robot.position.angle = get_pos()->angle;
	robot.pathLength = 0;
}

void robotUpdate () {
	robot.position.x = get_pos()->x;
	robot.position.y = get_pos()->y;
	robot.position.angle = get_pos()->angle;
}

int calculateDistance (POSITION origin, POSITION target) {
	int distance = Sqrt(Sqr(target.x - origin.x) + Sqr (target.y - origin.y));
	return distance;
}

int MAX (int a, int b) {
	return (a > b) ? a : b;
}

int MIN (int a, int b) {
	return (a < b) ? a : b;
}

int calculatePathVelocity (Path path, Robot robot) {
	int magnitude = 0;
	int distance = calculateDistance (path.position, robot.position);
	
	if(path.vel != 0) magnitude = path.vel;
	else {
		magnitude = MIN(100, MAX(6, 100 * (distance / 1500)));
		if(distance >1250) {
			magnitude = 65;
		}
	}
	return magnitude;	
}

int calculatePathBearing(Path path, Robot robot) {
	//TODO
}

int calculatePathAngularVelocity (Path path, Robot robot) {
	int magnitude  = getAngleDifference(path.position.angle, robot.position.angle) / 180 * 100;
	if (magnitude >= 1) magnitude = MAX(35, magnitude);
	return magnitude;
}

int binomial (int n, int k) {
	if (k> (n-k)) {
		k = n - k;
	}
	int b = 1;
	int m = n;
	for (int i = 1; i<=k; i++, m--) {
		b = b * m / i;
	}
	return b;
}

int getAngleDifference(int origin, int target) {
	int diff = origin - target;
	if (diff < -1800) diff += 3600;
	if (diff > 1800) diff -= 3600;
	return diff;
}

void updateQueue () {
	if(size == 0) {
		Path currentPath = queue [0];
		
		int currentDistance = calculateDistance(currentPath.position, robot.position);
		int currentAngle = Abs(getAngleDifference(currentPath.position.angle, robot.position.angle));
		
		if(allowArm){
			armUpdate();
		}
		
		if(robot.pathLength == -1) {
			robot.pathLength = calculateDistance(robot.start, currentPath.position);
		}
		
		if(currentDistance <= currentPath.distanceThreshold && currentAngle <= currentPath.bearingThreshold) {
			sendWheelBaseMotorCommands (0,0,0);
			dequeue(size);
			
			robot.pathLength = -1;
			robot.start.x = get_pos()->x;
			robot.start.y = get_pos()->y;
			robot.start.angle = get_pos()->angle;
		}
		else {
			int magnitude = calculatePathVelocity(currentPath, robot);
			int translationBearing = calculatePathBearing(currentPath, robot);
			int angularVelocity = calculatePathAngularVelocity(currentPath, robot);
			
			sendMotorCommands();
		}
	}
}

void dequeue (int size) {
	for(int i = 0; i<size-1; i++) {
		queue[i] = queue[i+1];
	}
	size--;
}
#include "control.h"
#include "pk/pk_movement.h"

#define PI 3.14159265359
#define MAX_MOTOR_PWM 160

Robot robot;
Path queue [40];
int size= 0, dispCurrentDistance = 0, dispCurrentBearing = 0, dispW = 0, timediff = 0;

void calculatePIDMotorValues(int vel, int bearing, int w){
	setM(vel);
	setBearing(bearing);
	setW(w);
	addComponent();
	parseMotorValues();
	sendMotorCommands();
}

void robotInit() {
	robot.position.x = get_pos()->x;
	robot.position.y = get_pos()->y;
	robot.position.angle = get_pos()->angle/10;
	robot.pathLength = 0;
}

void robotUpdate () {
	robot.position.x = get_pos()->x;
	robot.position.y = get_pos()->y;
	robot.position.angle = get_pos()->angle/10;
}

int calculateDistance (POSITION origin, POSITION target) {
	int distance = Sqrt(Sqr(target.x - origin.x) + Sqr (target.y - origin.y));
	return distance;
}

//int MAX (int a, int b) {
	//return (a > b) ? a : b;
//}
float MAX (float a, float b) {
	return (a > b) ? a: b;
}

//int MIN (int a, int b) {
	//return (a < b) ? a : b;
//}
float MIN (float a, float b) {
	return ( a < b) ? a : b;
}

int calculatePathVelocity (Path path, Robot robot) {
	float magnitude = 0;
		int distance = calculateDistance (path.position, robot.position);
		
		if(path.vel != 0) magnitude = path.vel;
		else {
			magnitude = MIN(75.0, MAX(10, 75.0 * (distance / (float)1000)));
			if(distance >1250) {
				magnitude = 65;
			}
		}
	return magnitude;	
}

int calculatePathBearing(Path path, Robot robot) {
//	int lineDiffX = path.position.x - robot.start.x;
//	int lineDiffY = path.position.y - robot.start.y;
//	int mouseDiffX = robot.position.x - robot.start.x;
//	int mouseDiffY = robot.position.y - robot.start.y;
//	int normDst = (lineDiffX * mouseDiffX + lineDiffY * mouseDiffY) / Sqr(calculateDistance(robot.start, path.position));
//	
//	int closestPathX = robot.start.x + lineDiffX * normDst;
//	int closestPathY = robot.start.y + lineDiffY * normDst;
//	int idealPointX = (closestPathX + path.position.x) / 2; 
//	int idealPointY = (closestPathY + path.position.y) / 2;
//	int bearing = 90 - int_arc_tan2(idealPointY - robot.position.y, idealPointX - robot.position.x);
//	bearing -= robot.position.angle;
//	if (bearing < -180) bearing += 360;
//	if (bearing > 180) bearing -= 360;
//	return bearing;
	
	int bearing = 90 - int_arc_tan2(path.position.y - robot.position.y, path.position.x - robot.position.x);
	bearing = bearing - robot.position.angle;
	if (bearing < -180)
	{
		bearing = bearing + 360;
	}
	if (bearing > 180)
	{
		bearing = bearing - 360;
	}
	return bearing;
	
}

int calculatePathAngularVelocity (Path path, Robot robot) {
	int magnitude = 0;
	float fmagnitude  = getAngleDifference(path.position.angle, robot.position.angle) * 100 / (float)180.0;
	if (fmagnitude >= 2) magnitude = MAX(35, fmagnitude);
	else if (fmagnitude <= -2) magnitude = MIN (-35, fmagnitude);
	else {
		magnitude = fmagnitude;
	}
	dispW = magnitude;
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
	if (diff < -180) diff += 360;
	if (diff > 180) diff -= 360;
	return diff;
}

long lastWait = -1;
bool currentOscDir = false;
int baseAngle = -1;

void updateQueue () {
	if(size != 0) {
		Path currentPath = queue [0];
		
		int currentDistance = calculateDistance(currentPath.position, robot.position);
		int currentAngle = Abs(getAngleDifference(currentPath.position.angle, robot.position.angle));
		dispCurrentDistance = currentDistance;
		dispCurrentBearing = currentAngle;
		
		if(allowArm){
			armUpdate();
		}
		
		if(robot.pathLength == -1) {
			robot.pathLength = calculateDistance(robot.start, currentPath.position);
		}
		
		if(currentDistance <= currentPath.distanceThreshold && currentAngle <= currentPath.bearingThreshold) {
			sendWheelBaseMotorCommands (0,0,0);
			if(currentPath.brushlessSpeed != -1) {
				setBrushlessMagnitude(currentPath.brushlessSpeed);
			}
			if(currentPath.waitTime <= 0) {
				dequeue(size);
				lastWait = -1;
			} else{
				if (lastWait == -1) lastWait= get_full_ticks();
				if (baseAngle == -1) {
					baseAngle = robot.position.angle;
					setM(0);
					setBearing(0);
					setW(currentOscDir ? 10 : -10);
					currentOscDir = !currentOscDir;
					addComponent();
					parseMotorValues();
					sendMotorCommands();
				}
				int dt = get_full_ticks() - lastWait;
				
				if (dt >= 0 && dt < 1500) {
					setBrushlessMagnitude(10); //12
				} else if (dt >= 1500 && dt < 3000) {
					setBrushlessMagnitude(18); //18
				} else if (dt >= 3000 && dt < 4500) {
					setBrushlessMagnitude(26); //26
				} else {
					setBrushlessMagnitude(30); //30
				}
				
				if (Abs(robot.position.angle - baseAngle) >= 5) {
					setM(0);
					setBearing(0);
					setW(currentOscDir ? 10 : -10);
					currentOscDir = !currentOscDir;
					addComponent();
					parseMotorValues();
					sendMotorCommands();
				}
				
				if (dt >= currentPath.waitTime) {
					lastWait = get_full_ticks();
					baseAngle = -1;
					setBrushlessMagnitude(0);
					dequeue(size);
				}
				
			}
//			else if(currentPath.waitTime == 6000) {
//				currentPath.clockTick = get_full_ticks();
//				currentPath.waitTime++;
//			}
//			else if(currentPath.waitTime == 6001) {
//				if(get_full_ticks() - currentPath.clockTick <= currentPath.waitTime) {
//					if(get_full_ticks() - currentPath.clockTick <= 1500) setBrushlessMagnitude(6);
//					else if(get_full_ticks() - currentPath.clockTick <= 3000) setBrushlessMagnitude(12);
//					else if(get_full_ticks() - currentPath.clockTick <= 4500) setBrushlessMagnitude(18);
//					else setBrushlessMagnitude(24);
//				}
//				else{
//					dequeue(size);
//				}
//			}
			
			robot.pathLength = -1;
			robot.start.x = get_pos()->x;
			robot.start.y = get_pos()->y;
			robot.start.angle = get_pos()->angle;
		}
		else {
			int magnitude = calculatePathVelocity(currentPath, robot);
			int translationBearing = calculatePathBearing(currentPath, robot);
			int angularVelocity = calculatePathAngularVelocity(currentPath, robot);
			
			calculatePIDMotorValues(magnitude, translationBearing, angularVelocity);
		}
	}
}

void dequeue (int _size) {
	if (size == 0) return;
	for(int i = 0; i<_size-1; i++) {
		queue[i] = queue[i+1];
	}
	size--;
}

void queueTargetPoint(int x, int y, int bearing, float thres, float bearThres, int brushlessSpeed, int waitTime) {
	queue[size].position.x = x;
	queue[size].position.y = y;
	queue[size].position.angle = bearing;
	queue[size].distanceThreshold = thres;
	queue[size].bearingThreshold = bearThres;
	queue[size].vel = 0;
	queue[size].brushlessSpeed = brushlessSpeed;
	queue[size].waitTime = waitTime;
	size++;
}

int getSize() {
	return size;
}

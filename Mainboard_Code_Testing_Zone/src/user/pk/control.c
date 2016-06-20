#include "control.h"
#include "pk_wheelbase.h"
#include "robocon.h"
#include "hybrid_laser.h"

#define PI 3.14159265359
#define MAX_MOTOR_PWM 200

Robot robot;
Path queue [40];
int size= 0, dispCurrentDistance = 0, dispCurrentBearing = 0, dispW = 0, dispM =0, timediff = 0, time = 0, blowTime = 0;
int expectRetry = 0;
bool finishing = false;

/**
	* @brief Calculates the motor speeds for the Wheelbase motors based on target M, bearing and W
	* @params int vel, bearing, w
	*/

void calculatePIDMotorValues(int vel, int bearing, int w){
	setM(vel);
	setBearing(bearing);
	setW(w);
	addComponent();
	parseWheelbaseValues();
	//sendWheelbaseCommand();
}

/**
	* @brief Initializes the robot structure that is storing information related to the robot
	*/

void robotInit() {
	robot.position.x = get_pos()->x;
	robot.position.y = get_pos()->y;
	robot.position.angle = get_pos()->angle/10;
	robot.pathLength = 0;
}

/**
	* @brief Updates data on the robot
	*/

void robotUpdate () {
	robot.position.x = get_pos()->x;
	robot.position.y = get_pos()->y;
	robot.position.angle = get_pos()->angle/10;
}

/**
	* @brief Calculates the distance between two points
	* @params POSITION origin and target
	* @retval int distance between the origin and target
	*/

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

/////////DO NOT USE THESE MAX MIN FUNCTIONS, USE THE ONE IN PK_MATH

/**
	* @brief Calculates the velocity required to reach the target point from robot current position
	* @params Path path and Robot robot
	* @retval int magnitude of the velocity or M
	*/

int calculatePathVelocity (Path path, Robot robot) {
	if(path.distanceThreshold != -1 && path.bearingThreshold != -1){
	float magnitude = 0;
		int distance = calculateDistance (path.position, robot.position);
		
		if(path.vel != 0) magnitude = path.vel;
		else {
			magnitude = MIN(50.0, MAX(10, 50.0 * (distance / (float)1000)));
			if(distance >1250) {
				magnitude = 40;
			}
		}
		dispM = magnitude;
	return magnitude;	
	}
	else {
		dispM = 60;
		return 60;
	}
}

/**
	* @brief Calculates the bearing required to reach the target point from robot current position
	* @params Path path and Robot robot
	* @retval int bearing
	*/

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

/**
	* @brief Calculates the angular velocity required to reach the target point from robot current position
	* @params Path path and Robot robot
	* @retval int magnitude of the angular velocity or W
	*/

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

/**
	* @brief Returns the angle difference between the two inputs origin and target, in bearing, unscaled
	* @params int origin and target
	* @retval int difference
	*/

int getAngleDifference(int origin, int target) {
	int diff = origin - target;
	if (diff < -180) diff += 360;
	if (diff > 180) diff -= 360;
	return diff;
}

long lastWait = -1;
bool currentOscDir = false;
int baseAngle = -1;

/**
	* @brief Updating the queue, standard queue operations for auto PID movement executed here
	*/

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
		
		if((currentDistance <= (currentPath.distanceThreshold == -1 ? 400 : currentPath.distanceThreshold)
			&& currentAngle <= (currentPath.bearingThreshold == -1 ? 240 : currentPath.bearingThreshold )) || finishing) {
			
			if(currentPath.waitTime <= 0) {
				if(finishing == true) finishing = false;
				else {
				//sendWheelBaseMotorCommands(0,0,0);
				//wheelbaseLock();
					if(currentPath.brushlessSpeed != -1) {
						setBrushlessMagnitude(currentPath.brushlessSpeed);
					}
					dequeue(size);
					lastWait = -1;
				}
			} else{
				finishing = true;
				wheelbaseLock();
				if (lastWait == -1) {
					time = currentPath.waitTime;
					lastWait= get_full_ticks();
				}
				//if (baseAngle == -1) {
					//baseAngle = robot.position.angle;
					//setM(0);
					//setBearing(0);
					//setW(currentOscDir ? 10 : -10);
					//currentOscDir = !currentOscDir;
					//addComponent();
					//parseWheelbaseValues();
					//sendWheelbaseCommand();
				//}
				int dt = get_full_ticks() - lastWait;
				
				blowTime = dt;
				if(currentPath.position.y == wagateki) {
					//retryProcedureCheck();
					
					//setBrushlessMagnitude(14); //12
					
					if(dt <= time /4) {
						setBrushlessMagnitude(robotMode == RED_SIDE ? 11 : 12);
					}
					else if (dt > time/4) {
						setBrushlessMagnitude(robotMode == RED_SIDE ? 14 : 22);
					}
				//else if (dt >= time/2 && dt < time*3/4) {
				//	setBrushlessMagnitude(10); //18 
				//} 
//				else if (dt >= time && dt < time*3/4) {
//					setBrushlessMagnitude(35); //26
			//}
//					else {
//						setBrushlessMagnitude(25); //30
//					}
				}
				else if(currentPath.position.y == savedY) {
					if(dt >= 0 && dt < time *3 / 4) {
						setBrushlessMagnitude(12); //8
					}
					else {
						setBrushlessMagnitude(18); //23
					}
				}
				
//				if (Abs(robot.position.angle - baseAngle) >= 5) {
//					setM(0);
//					setBearing(0);
//					setW(currentOscDir ? 10 : -10);
//					currentOscDir = !currentOscDir;
//					addComponent();
//					parseWheelbaseValues();
//					sendWheelbaseCommand();
//				}
				
				if (dt >= currentPath.waitTime) {
					lastWait = -1;
					baseAngle = -1;
					
					//Conditions for after reaching the blowing position
					//TODO: ADD CONDITION FOR RED_SIDE
					if(currentPath.position.y == wagateki) {
						setBrushlessMagnitude(0);
						currMode = RETRYCHECK;
						expectRetry = get_full_ticks();
					}
					wheelbaseLock();
					//finishing = false;
					dequeue(size);
					
					//Exit PID Mode, goto Laser Mode --> after reaching first point for either side of gamefield
					//TODO: ADD CONDITION FOR RED SIDE
					if(currentPath.position.x == savedX && currentPath.position.y == savedY) {
						currMode = LASERPID;
						setBrushlessMagnitude(currentPath.brushlessSpeed);
					}
				}
				
			}
			
			robot.pathLength = -1;
			robot.start.x = get_pos()->x;
			robot.start.y = get_pos()->y;
			robot.start.angle = get_pos()->angle;
		}
		else {
			int magnitude = calculatePathVelocity(currentPath, robot);
			int translationBearing = calculatePathBearing(currentPath, robot);
			int angularVelocity = calculatePathAngularVelocity(currentPath, robot);
			
			if(currentPath.position.y == wagateki && currentPath.position.x == wagamama && currentDistance < currentPath.distanceThreshold) {
				//magnitude = 0;
				//if(Abs(getAngleDifference(robot.position.angle, currentPath.position.angle)) > 10) angularVelocity = 2* angularVelocity;
			}
			calculatePIDMotorValues(magnitude, translationBearing, angularVelocity);
		}
	}
	else {
		currMode = MANUAL;
	}
}

/**
	* @brief Dequeue the topmost item in the queue
	*/

void dequeue (int _size) {
	if (size == 0) return;
	for(int i = 0; i<_size-1; i++) {
		queue[i] = queue[i+1];
	}
	size--;
}

/**
	* @brief Dequeue all items in the queue
	*/

void dequeueAll() {
	for(int i = getSize(); i>=1; i--) {
			dequeue(getSize());
	}
}

/**
	* @brief Queue an item to the queue
* @param Integers: xCoord, yCoord, bearing (degrees and no scaling), brushlessSpeed = -1 for ignore prev speed, waitTime : -1 if doesn't exist
* 		 , Float: distanceThreshold and bearingThreshold : for how close the robot should go to a certain point, -1 for ignore threshold
	*/

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

/**
	* @brief Returns the size of the queue
* @retval int: size of the queue
	*/
int getSize() {
	return size;
}

double calculateAngularVelocity(int targAngle, int max, int min, int scale) {
	double angularVelocity = getAngleDifference(robot.position.angle, targAngle) * scale / 180 * -1;
		if (Abs(angularVelocity) >= max) angularVelocity = angularVelocity < 0 ? -max : max;
		else{
			if (angularVelocity > 0) angularVelocity = MAX(min, angularVelocity);
			if (angularVelocity < 0) angularVelocity = MIN(-min, angularVelocity);
		}
		return angularVelocity;
}

void waitingForRetry (void) {
	if(get_full_ticks() - expectRetry >= 3000) {
		currMode = APPROACHWALL;
		allowArm = false;
	}
	else{
		retryProcedureCheck();
	}
}

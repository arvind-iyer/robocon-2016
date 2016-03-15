#include "approx_math.h"

#define PI 3.1415926535897932

#define MAX_VELOCITY 140
#define MAX_ROTATION 80
#define STOP_DISTANCE 2000
#define ERR_INCREMENT 0.01
#define ERR_DECREMENT 0.2
#define ACCELERATION 10

typedef struct
{
	int x;
	int y;
}
Vector;

Vector newVector(int x, int y)
{
	Vector temp;
	temp.x = x;
	temp.y = y;
	return temp;
}

typedef struct
{
	Vector vector;
	int orientation;
}
Position;

Position newPosition(Vector vector, int orientation)
{
	Position temp;
	temp.vector = vector;
	temp.orientation = orientation;
	return temp;
}

typedef struct
{
	Position position;
	int velocity;
}
Target;

Target newTarget(Position position, int velocity)
{
	Target temp;
	temp.position = position;
	temp.velocity = velocity;
	return temp;
}

typedef struct
{
	int M1;
	int M2;
	int M3;
}
Motors;

Motors newMotors(int M1, int M2, int M3)
{
	Motors temp;
	temp.M1 = M1;
	temp.M2 = M2;
	temp.M3 = M3;
	return temp;
}

int pathDist(Vector origin, Vector target)
{
	return (int) (Sqrt(Sqr(target.y - origin.y) + Sqr(target.x - origin.x)));
}

int targetBearing(Position origin, Vector target)
{
	int bearing = 90 - int_arc_tan2(target.y - origin.vector.y, target.x - origin.vector.x);
	bearing = bearing - origin.orientation;
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

int angleDiff(int origin, int target)
{
	int diff = target - origin;
	if (diff < -180)
	{
		diff = diff + 360;
	}
	if (diff > 180)
	{
		diff = diff - 360;
	}
	return diff;
}

Motors motors;
Motors lastMotors;

Position currentPos;

Target target;
Vector targetVector;
int targetOrientation;
int targetVelocity;

int lastM;
int lastBearing;
int lastW;

int lastDist;
int lastAngleDiff;

float errDist;
float errAngle;

void init()
{
	motors = newMotors(0, 0, 0);
	lastMotors = motors;
	currentPos = newPosition(newVector(0, 0), 0);
	target = newTarget(currentPos, 0);
	targetVector = currentPos.vector;
	targetOrientation = currentPos.orientation;
	targetVelocity = 0;
	lastM = 0;
	lastBearing = 0;
	lastW = 0;
	lastDist = 0;
	lastAngleDiff = 0;
	errDist = 1.00;
	errAngle = 1.00;
}

int getMotor1()
{
	lastMotors.M1 = motors.M1;
	return motors.M1;
}

int getMotor2()
{
	lastMotors.M2 = motors.M2;
	return motors.M2;
}

int getMotor3()
{
	lastMotors.M3 = motors.M3;
	return motors.M3;
}

void updateCurrentPos(Position pos)
{
	currentPos = pos;
}

void updateTarget()
{
	target = newTarget(newPosition(targetVector, targetOrientation), targetVelocity);
}

void lockTarget(Vector vector, int orientation, int velocity)
{
	targetVector = vector;
	targetOrientation = orientation;
	targetVelocity = velocity;
	updateTarget();
	errDist = 1.00;
	errAngle = 1.00;
}

void lockCurrentVector()
{
	targetVector = currentPos.vector;
	targetVelocity = 0;
	updateTarget();
	errDist = 1.00;
}

void lockCurrentOrientation()
{
	targetOrientation = currentPos.orientation;
	targetVelocity = 0;
	updateTarget();
	errAngle = 1.00;
}

void generatePID_M()
{
	int M = 0;
	if (targetVelocity != 0)
	{
		M = targetVelocity;
	}
	else
	{
		M = Sqr(pathDist(currentPos.vector, target.position.vector)) * 100 / Sqr(STOP_DISTANCE);
		if (pathDist(currentPos.vector, target.position.vector) != 0 && M == 0)
		{
			M = 1;
		}
	}
	lastM = M;
}

void generatePID_bearing()
{
	lastBearing = targetBearing(currentPos, target.position.vector);
}

void generatePID_W()
{
	int W = Sqr(angleDiff(currentPos.orientation, target.position.orientation)) * 100 / Sqr(180);
	if (angleDiff(currentPos.orientation, target.position.orientation) != 0 && W == 0)
	{
		if (angleDiff(currentPos.orientation, target.position.orientation) > 0)
		{
			W = 1;
		}
		else
		{
			W = -1;
		}
	}
	lastW = W;
}

void overrideM(int M)
{
	lastM = M;
	errDist = 1.00;
}

void overrideBearing(int bearing)
{
	lastBearing = bearing;
}

void overrideW(int W)
{
	lastW = W;
	errAngle = 1;
}

void tuneErrM()
{
    int currentDist = pathDist(currentPos.vector, target.position.vector);
    if (currentDist >= lastDist && currentDist != 0)
	{
        errDist = errDist + ERR_INCREMENT;
    }
	else
	{
        errDist = errDist * (1 - ERR_DECREMENT) + ERR_DECREMENT;
    }
    if (errDist > 20)
	{
        errDist = 20;
    }
}

void tuneErrW()
{
    int currentAngleDiff = Abs(angleDiff(currentPos.orientation, target.position.orientation));
    if (currentAngleDiff >= lastAngleDiff && currentAngleDiff != 0)
	{
        errAngle = errAngle + ERR_INCREMENT;
    }
	else
	{
        errAngle = errAngle * (1 - ERR_DECREMENT) + ERR_DECREMENT;
    }
    if (errAngle > 100)
	{
        errAngle = 100;
    }
}

void parseMotorValues()
{
    int finalM = lastM * errDist;
    int finalW = lastW * errAngle;
    int xComponent = int_sin(lastBearing * 10) * MAX_VELOCITY * finalM / 100 / 10000;
    int yComponent = int_cos(lastBearing * 10) * MAX_VELOCITY * finalM / 100 / 10000;
    int M1 = (-finalW - xComponent * 2) / 3;
    int M2 = ((-finalW * 0.5774 + xComponent * 0.5774 - yComponent) / 1.7321);
    int M3 = -finalW - M1 - M2;
    motors.M1 = M1;
    motors.M2 = M2;
    motors.M3 = M3;
}

void accelClamp()
{
	int M1_diff = motors.M1 - lastMotors.M1;
    int M2_diff = motors.M2 - lastMotors.M2;
    int M3_diff = motors.M3 - lastMotors.M3;
    int max;
    max = Abs(M1_diff);
    if (Abs(M2_diff) > max)
    {
    	max = Abs(M2_diff);
	}
	if (Abs(M3_diff) > max)
	{
		max = Abs(M3_diff);
	}
    int clampFactor = max / ACCELERATION;
    if (clampFactor < 1)
	{
        clampFactor = 1;
    }
    motors.M1 = lastMotors.M1 + M1_diff / clampFactor;
    motors.M2 = lastMotors.M2 + M2_diff / clampFactor;
    motors.M3 = lastMotors.M3 + M3_diff / clampFactor;
}

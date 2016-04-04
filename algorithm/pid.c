#include "pid.h"
#include <math.h>

#define PI 3.1415926535897932


Vector newVector(int x, int y)
{
	Vector temp;
	temp.x = x;
	temp.y = y;
	return temp;
}

Position newPosition(Vector vector, int orientation)
{
	Position temp;
	temp.vector = vector;
	temp.orientation = orientation;
	return temp;
}

Target newTarget(Position position, int velocity)
{
	Target temp;
	temp.position = position;
	temp.velocity = velocity;
	return temp;
}

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
	return (int) (sqrt(pow(target.y-origin.y, 2) + pow(target.x-origin.x, 2)));
}

int targetBearing(Position origin, Vector target)
{
	int bearing = 90 - atan2(target.y - origin.vector.y, target.x - origin.vector.x) * 180 / PI;
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



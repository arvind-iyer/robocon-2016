#include <math.h>

#define PI 3.1415926535897932
#define WEIGHT 1

typedef struct
{
	int x;
	int y;
	int magnitude;
	int bearing;
}
Vector;

typedef struct
{
	Vector vector;
	int orientation;
}
Position;

Vector newVector(int x, int y)
{
	Vector temp;
	temp.x = x;
	temp.y = y;
	temp.magnitude = sqrt(x * x + y * y);
	temp.bearing = 90 - atan2(y, x) * 180 / PI;
	if (temp.bearing < -180)
	{
		temp.bearing = temp.bearing + 360;
	}
	if (temp.bearing > 180)
	{
		temp.bearing = temp.bearing - 360;
	}
	return temp;
}

Vector vectorSum(Vector a, Vector b)
{
	int x = a.x + b.x;
	int y = a.y + b.y;
	return newVector(x, y);
}

Position newPosition(Vector vector, int orientation)
{
	Position temp;
	temp.vector = vector;
	temp.orientation = orientation;
	return temp;
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

int pathDist(Vector origin, Vector target)
{
	return (int) (sqrt(pow(target.y-origin.y, 2) + pow(target.x-origin.x, 2)));
}

Vector projection(Vector start, Vector end, Vector current)
{
	float x1 = start.x;
	float y1 = start.y;
	float x2 = end.x;
	float y2 = end.y;
	float x3 = current.x;
	float y3 = current.y;
	float k = ((y2-y1) * (x3-x1) - (x2-x1) * (y3-y1)) / ((y2-y1)*(y2-y1) + (x2-x1)*(x2-x1));
	float x4 = x3 - k * (y2-y1);
	float y4 = y3 + k * (x2-x1);
	return newVector(x4, y4);
}

int main()
{
	Position currentPos = newPosition(newVector(500, 500), 0);
	Vector startPoint = newVector(0, 0);
	Position targetPos = newPosition(newVector(0, 1000), 0);
	Vector D = projection(startPoint, targetPos.vector, currentPos.vector);
	Vector correctionVector = vectorSum(newVector(-currentPos.vector.x, -currentPos.vector.y), D);
	printf("correctionVector = %d [%d]\n", correctionVector.magnitude, correctionVector.bearing);
	Vector pathVector = vectorSum(newVector(-currentPos.vector.x, -currentPos.vector.y), targetPos.vector);
	printf("pathVector = %d [%d]\n", pathVector.magnitude, pathVector.bearing);
	Vector finalVector = vectorSum(newVector(WEIGHT * correctionVector.x, WEIGHT * correctionVector.y), pathVector);
	printf("Final Bearing = [%d]\n", finalVector.bearing - currentPos.orientation);
}


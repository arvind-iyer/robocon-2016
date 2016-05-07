/**
* The Big Plan
* 
* bearing should only lie within -90 to 90
* M can be negative
* 
*/

#include "pk_positioning.h"
#include "gyro.h"

Vector newVector(int x, int y) {
	Vector temp;
	temp.x = x;
	temp.y = y;
	return temp;
}

Position newPosition(Vector vector, int orientation) {
	Position temp;
	temp.vector = vector;
	temp.orientation = orientation;
	return temp;
}

Position getCurrentPos() {
	int x = get_pos()->x;
	int y = get_pos()->y;
	int angle = get_pos()->angle / 10;
	if (angle > 180) {
		angle = angle - 360;
	}
	return newPosition(newVector(x, y), angle);
}

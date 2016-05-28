#include "pk_math_vector.h"
#include "approx_math.h"

Vector newVector(int x, int y, int z) {
	Vector temp;
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

int dot_product(Vector a, Vector b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector cross_product(Vector a, Vector b) {
	return newVector(a.y * b.z - b.y * a.z, a.z * b.x - b.z * a.x, a.x * b.y - a.y * b.x);
}

int getMagnitude(Vector a) {
	return Sqrt(Sqr(a.x) + Sqr(a.y) + Sqr(a.z));
}

int getAngle(Vector a, Vector b) {
	return int_arc_cos(getMagnitude(cross_product(a, b)) * 10000 / (getMagnitude(a) * getMagnitude(b)));
}

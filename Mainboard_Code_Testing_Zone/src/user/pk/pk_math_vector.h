#ifndef _PK_MATH_VECTOR
#define _PK_MATH_VECTOR

typedef struct {
	int x;
	int y;
	int z;
} Vector;

Vector newVector(int x, int y, int z);

int dot_product(Vector a, Vector b);
Vector cross_product(Vector a, Vector b);

int getMagnitude(Vector a);

int getAngle(Vector a, Vector b);

#endif

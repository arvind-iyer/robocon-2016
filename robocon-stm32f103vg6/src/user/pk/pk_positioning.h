#ifndef _PK_POSITIONING
#define _PK_POSITIONING

typedef struct {
	int x;
	int y;
} Vector;

typedef struct {
	Vector vector;
	int orientation;
} Position;

Vector newVector(int x, int y);
Position newPosition(Vector vector, int orientation);

Position getCurrentPos(void);

#endif

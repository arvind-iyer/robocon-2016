typedef struct
{
	int x;
	int y;
}
Vector;

typedef struct
{
	Vector vector;
	int orientation;
}
Position;

typedef struct
{
	Position position;
	int velocity;
}
Target;

typedef struct
{
	int M1;
	int M2;
	int M3;
}
Motors;


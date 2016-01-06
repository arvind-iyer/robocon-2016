#include <iostream>
#include <math.h>

#define PI 3.1415926535897932
#define MAXVEL 50

using namespace std;

void _move(float X, float Y, float W)
{
	if (sqrt(X*X+Y*Y)>MAXVEL)
	{
		Y=sqrt(MAXVEL*MAXVEL/(1+X*X/Y/Y));
		X=sqrt(MAXVEL*MAXVEL-Y*Y);
	}
	printf("_move(%.2f, %.2f, %.2f)\n", X, Y, W);
	int M1;
	int M2;
	int M3;
	M1=(-W-X*2)/3;
	M2=(-W*sqrt(3)/3+X*sqrt(3)/3-Y)/sqrt(3);
	M3=-W-M1-M2;
	printf("M1: %d\nM2: %d\nM3: %d\n\n", M1, M2, M3);
}

void _move(int magnitude, int bearing)
{
	_move(magnitude*sin(bearing*PI/180), magnitude*cos(bearing*PI/180), 0);
}

int main()
{
	while (1)
	{
		
		int x;
		int y;
		int w;
		scanf("%d %d %d", &x, &y, &w);
		_move(x, y, w);
		
		/*
		int m;
		int b;
		scanf("%d %d", &m, &b);
		_move(m, b);
		*/
	}
}

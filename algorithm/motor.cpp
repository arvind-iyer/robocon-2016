#include <iostream>
#include <math.h>

#define PI 3.1415926535897932

using namespace std;

void _move(float X, float Y, float W)
{
	int M1;
	int M2;
	int M3;
	M1 = X*2+W;
	M2 = (Y-X*sqrt(3))/sqrt(3);
	M3 = -W-M1-M2;
	//printf("_move\t\tM1: %d\tM2: %d\tM3: %d\n", M1, M2, M3);
}

void _move(int magnitude, int bearing)
{
	_move(magnitude*sin(bearing*PI/360), magnitude*cos(bearing*PI/360), 0);
}

int main()
{
	while (1)
	{
		int m;
		int b;
		scanf("%d %d", &m, &b);
		_move(m, b);
	}
}

#include <stdbool.h>

#define MAX_VELOCITY 140
#define MAX_ROTATION 80
#define STOP_DISTANCE 2000
#define ERR_INCREMENT 0.01
#define ERR_DECREMENT 0.7
#define ACCELERATION 5

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

Vector newVector(int x, int y);
Position newPosition(Vector vector, int orientation);
Target newTarget(Position position, int velocity);
Motors newMotors(int M1, int M2, int M3);

int pathDist(Vector origin, Vector target);
int targetBearing(Position origin, Vector target);
int angleDiff(int origin, int target);

void pid_init();

int getLastM();
int getLastBearing();
int getLastW();

float getErrDist();
float getErrAngle();

int getMotor1();
int getMotor2();
int getMotor3();

Target getLockedTarget();

void updateCurrentPos(Position pos);

void updateTarget();
void lockTarget(Vector vector, int orientation, int velocity);
void lockCurrentVector();
void lockCurrentOrientation();

void generatePID_M();
void generatePID_bearing();
void generatePID_W();

void overrideM(int M);
void overrideBearing(int bearing);
void overrideW(int W);

void tuneErrM();
void tuneErrW();

void parseMotorValues();
void accelClamp();

bool completed(int dist, int angle);
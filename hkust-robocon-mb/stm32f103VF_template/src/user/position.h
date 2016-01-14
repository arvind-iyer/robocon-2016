#include "approx_math.h"
#include "pid.h"
#include "gyro.h"
#include "ticks.h"

#define MAX_VELOCITY 140
#define MAX_MAGNITUDE 100
#define NO_CURVE 99999

typedef struct Target {
	float x, y, angle, curveFactor;
} Target;

typedef struct TargetQueue {
  Target *array;
  uint32_t used;
  uint32_t size;
} TargetQueue;

extern float TARGET_X;
extern float TARGET_Y;

extern int TARGET_DIRECTION;
extern s32 TARGET_TICKS;

extern int ROBOT_MOVING;

extern Target curvePoint;

extern float velocities[3];
extern float err;

extern PID curvePid;

void pursueTarget();

void lockAllMotors();
void setRobotVelocity(float magnitude, float translationAngle, float angularVelocity);
void setTargetLocation(int x, int y, int angle);

void initTargetQueue(uint32_t initialSize);
void queueTarget(Target element);
void dequeueTarget();
void freeQueue();
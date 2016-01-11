#include "approx_math.h"
#include "pid.h"
#include "gyro.h"
#include "ticks.h"

#define MAX_MAGNITUDE 140
#define NO_CURVE 99999
#define ROBOT_VELOCITY 50

typedef struct Target {
	int32_t x, y, angle, curveFactor;
} Target;

typedef struct TargetQueue {
  Target *array;
  uint32_t used;
  uint32_t size;
} TargetQueue;

extern s32 TARGET_X;
extern s32 TARGET_Y;

extern s32 TARGET_DIRECTION;
extern s32 TARGET_TICKS;

extern int ROBOT_MOVING;

extern PID velXPid;
extern PID velYPid;
extern PID velWPid;
void pursueTarget();

void lockAllMotors();
void setRobotVelocity(int direction, int vel, int velW, int flag);
void setTargetLocation(int x, int y, int angle);

void initTargetQueue(uint32_t initialSize);
void queueTarget(Target element);
void dequeueTarget();
void freeQueue();
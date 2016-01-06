#include "approx_math.h"
#include "gyro.h"
#include "ticks.h"

#define MAX_VELOCITY 50

#define KP 0.86
#define KI 0.35
#define KD 1.24


#define ROBOT_VELOCITY 50

extern s32 TARGET_X;
extern s32 TARGET_Y;

extern s32 TARGET_DIRECTION;
extern s32 TARGET_TICKS;

extern s32 ROBOT_POS_X;
extern s32 ROBOT_POS_Y;
extern int ROBOT_MOVING;

void pursueTarget();

void lockAllMotors();
void setRobotVelocity(int direction, int velocity);
void setTargetLocation(int x, int y);
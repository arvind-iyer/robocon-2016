#define MAX_VELOCITY 50

#define X_KP 0.54
#define X_KI 0.015
#define X_KD 0.86

#define Y_KP 0.54
#define Y_KI 0.015
#define Y_KD 0.86

#define ROBOT_VELOCITY 50

extern int TARGET_X;
extern int TARGET_Y;

extern int TARGET_DIRECTION;

extern int ROBOT_POS_X;
extern int ROBOT_POS_Y;
extern int ROBOT_MOVING;

void pursueTarget();

void lockAllMotors();
void setRobotVelocity(int direction, int velocity);
void setTargetLocation(int x, int y);
#ifndef _HYBRID_GPIO
#define _HYBRID_GPIO

#include "stdbool.h"

void hybridGPIOInit(void);
void limitSwitchCheck(void);
void armUpdate(void);
bool retryIrChecking(void);
void retryProcedureCheck(void);
void grabPropeller(void);

bool getLS(int index);

extern int ctr;
extern int moveDelay;
extern bool retryIr;
extern bool prevRetryIr;
extern bool fixingArm;
extern bool allow4thUpdate;
extern int climbingState;
extern bool retrySpeed;

#endif

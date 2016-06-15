#ifndef _HYBRID_GPIO
#define _HYBRID_GPIO

#include "stdbool.h"

void hybridGPIOInit(void);
void limitSwitchCheck(void);
void armUpdate(void);

bool getLS(int index);

extern int moveDelay;
extern bool allow4thUpdate;

#endif

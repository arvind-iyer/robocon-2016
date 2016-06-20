#ifndef _PK_MANUAL
#define _PK_MANUAL

#define UNLOCK 0
#define LOCK 1

void manualControl(void);

int getTranslationMagnitude(void);
int getTranslationBearing(void);
int getRotationValue(void);

void setTargetAngle(int angle);
int generateWForTargetAngle(int angle);

void lockBearing(int state);

#endif

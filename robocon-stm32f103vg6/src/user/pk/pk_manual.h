#ifndef _PK_MANUAL
#define _PK_MANUAL

void manualControl(void);

int getTranslationMagnitude(void);
int getTranslationBearing(void);
int getRotationValue(void);

void setTargetAngle(int angle);
int generateWForTargetAngle(int angle);

#endif

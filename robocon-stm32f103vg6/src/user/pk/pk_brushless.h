#ifndef _PK_BRUSHLESS
#define _PK_BRUSHLESS

#define MIN_PWM 590
#define MAX_PWM 1050

void brushlessStartup();
int getBrushlessMagnitude();
void setBrushlessMagnitude(int m);

#endif
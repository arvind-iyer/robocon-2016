#ifndef _PK_PID
#define _PK_PID

#define PID_P 1
#define PID_I 0
#define PID_D 0

void pk_pid_init();

void setTargetValue(int target);
void takeReading(int reading);
void takeTimeElapsed(int time);

void generateErrP();
void generateErrI();
void generateErrD();

int getOutput();

#endif

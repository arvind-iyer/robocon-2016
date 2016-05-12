#ifndef _PK_WHEELBASE
#define _PK_WHEELBASE

#define MAX_VEL 160
#define ACCELERATION 10

typedef struct {
	int target;
	int sent;
} Motor;

typedef struct {
	Motor M1;
	Motor M2;
	Motor M3;
	int lastTick;
} Wheelbase;

void pk_wheelbase_init();

void parseMotorValues(int M, int bearing, int W);
void sendWheelbaseValues();

#endif

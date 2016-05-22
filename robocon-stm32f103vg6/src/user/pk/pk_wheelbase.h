#ifndef _PK_WHEELBASE
#define _PK_WHEELBASE

#define MAX_VEL 160
#define INTERVAL 100
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

void setM(int _M);
void setBearing(int _bearing);
void setW(int _W);
void addComponent();

void parseWheelbaseValues();
void sendWheelbaseCommand();

void wheelbaseLock();

Wheelbase getWheelbaseValues();

#endif

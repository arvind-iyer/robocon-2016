#ifndef _PK_WHEELBASE
#define _PK_WHEELBASE

#define MAX_VEL 200
#define INTERVAL 12
#define ACCELERATION 4


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

void pk_wheelbase_init(void);

void reset(void);

void setM(int _M);
void setBearing(int _bearing);
void setW(int _W);
void addComponent(void);

void parseWheelbaseValues(void);
void sendWheelbaseCommand(void);

void wheelbaseLock(void);

Wheelbase getWheelbaseValues(void);

#endif

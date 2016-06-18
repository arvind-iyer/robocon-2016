#ifndef _PK_ROBOCON_AUTO
#define _PK_ROBOCON_AUTO

//#include "control.h"

#define PNEUMATIC_POLE_GRIP 0
#define PNEUMATIC_POLE_RELEASE 1
#define PNEUMATIC_PROPELLER_GRIP 1
#define PNEUMATIC_PROPELLER_RELEASE 0
#define PNEUMATIC_SLIDER_IN 1
#define PNEUMATIC_SLIDER_OUT 0

#define LS_POLE 3
#define LS_CLIMB 2
#define LS_ARM_TOP 0

enum auto_state {
	PREPARATION = 0,
	MOVE_TO_POLE = 1,
	RAM_POLE = 2,
	GRIP_POLE = 3,
	GRAB_PROPELLER = 999,
	CLIMB_POLE = 4,
	RAISE_ARM = 4,
	INSTALL_PROPELLER = 5,
	LOWER_ARM = 6,
	RESTORE = 7
};

void recordTick(void);
int getTimer(void);

void auto_PREPARATION(void);
void auto_MOVE_TO_POLE(void);
void auto_RAM_POLE(void);
void auto_GRIP_POLE(void);
void auto_CLIMB_POLE(void);
void auto_RAISE_ARM(void);
void auto_INSTALL_PROPELLER(void);
void auto_LOWER_ARM(void);
void auto_RESTORE(void);

void auto_next(void);
void auto_execute(void);

#endif

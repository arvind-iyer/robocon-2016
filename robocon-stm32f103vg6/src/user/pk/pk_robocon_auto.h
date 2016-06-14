#ifndef _PK_ROBOCON_AUTO
#define _PK_ROBOCON_AUTO

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
	PREPARATION,
	MOVE_TO_POLE,
	GRIP_POLE,
	GRAB_PROPELLER,
	CLIMB_POLE,
	RAISE_ARM,
	INSTALL_PROPELLER,
	LOWER_ARM,
	RESTORE
};

void auto_PREPARATION();
void auto_MOVE_TO_POLE();
void auto_GRIP_POLE();
void auto_CLIMB_POLE();
void auto_RAISE_ARM();
void auto_INSTALL_PROPELLER();
void auto_LOWER_ARM();
void auto_RESTORE();

void auto_next();
void auto_execute();

#endif

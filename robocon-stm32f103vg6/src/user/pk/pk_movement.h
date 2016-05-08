#ifndef _PK_MOVEMENT
#define _PK_MOVEMENT

#define MAX_VEL 140

typedef struct {
	int M1;
	int M2;
	int M3;
	int M4;
	int M5;
	int M6;
	int M7;
	int M8;
} Motors;



void pk_movement_init();
void setM(int _M);
void setBearing(int _Bearing);
void setW(int _W);
void addComponent();
void parseMotorValues();

void sendClimbCommands(int M4, int M5, int M6, int M7);
void sendMotorCommands();
void sendArmCommand(int M8);

Motors getMotorValues();

#endif

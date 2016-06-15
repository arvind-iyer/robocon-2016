#ifndef __HYBRID_LASER_H
#define __HYBRID_LASER_H

#define TICKS_DELAY 200

#include "laser_sensor.h"
#include "robocon.h"
#include "approx_math.h"
#include "stdbool.h"
#include "pk_math.h"

extern int increment;
extern int haha;
extern int laserB;
extern int laserW;
extern bool fieldDetected;
extern int wagateki;
extern int savedX;
extern int savedY;
extern int slowdownDelay;
extern int wagamama;
//extern u16 ls_cal_reading[2];

#define RED_SIDE 0
#define BLUE_SIDE 1


void dual_laser_init(void);
s16 get_cheap_laser_dis(u8 id);
void dual_laser_init_update(void);
bool is_dual_laser_ready(void);
void LaserHandlerFunction (u8 rx_data);

void enterPole(void);
void laserPID(void);
void moveToWall(void);
void laserCallbacks(STAGES stage);
void moveToFirstPosition(void);

#endif

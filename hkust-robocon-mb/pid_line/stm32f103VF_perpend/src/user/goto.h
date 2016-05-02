#include "can_motor.h"
#include "approx_math.h"
#include "gyro.h"
#include "uart.h"
#include "lcd_red.h"
#include "math.h"
#include "ticks.h"

void setTargetPos(int16_t x, int16_t y, int16_t theta);
void setRobotVelocity(int16_t bearing, int16_t magnitude, int16_t w);
void chase();
uint8_t isRunning();
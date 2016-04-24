#ifndef __BAJAJ
#define __BAJAJ

#include "stm32f4xx.h"
#include <stdbool.h>
#include "can_protocol.h"
#include "tft_display.h"
#include "ticks.h"
#include "infrared_sensor.h"
#include "servo.h"
#include "encoder.h"
#include "led.h"
#include "buzzer_song.h"
#include "buzzer.h"
#include "button.h"
#include "ardu_imu.h"
#include "approx_math.h"
#include <math.h>


#define BAJAJ
#ifdef BAJAJ
#define SERVO_MICROS_MID 2700
#define SERVO_MICROS_RIGHT 1900
#define SERVO_MICROS_LEFT 3500
#define BAJAJ_SERVO  SERVO3
#endif

enum{NOT_RIVER = 0, STAGE1 = 1, STAGE2 = 2, STAGE3 = 3};

enum{OFF = 0 , ON = 1};

void receive(CanRxMsg msg);
void receive2(CanRxMsg msg);
void receive3(CanRxMsg msg);

void process_array(void);

void print_data(void);
void fill_sensorbar_array(void);

void systemInit(void);

void goNormal(void);


void goUsingImu(void);

void goStraightLittleBit(void);



#endif




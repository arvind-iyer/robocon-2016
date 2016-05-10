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
#define SERVO_MICROS_MID 1200
#define SERVO_MICROS_RIGHT 800
#define SERVO_MICROS_LEFT 1800
#define BAJAJ_SERVO  SERVO1
#endif

enum{NOT_RIVER = 0, STAGE1 = 1, STAGE2 = 2, STAGE3 = 3};


typedef enum{
    ORANGEZONE = 0, DARKGREENZONE = 1, BLUEZONE = 2, LIGHTGREENZONE = 3, PINKZONE = 4, NOCOLOURZONE = 5
}ZONE;

enum{OFF = 0 , ON = 1};

void receive(CanRxMsg msg);
void receive2(CanRxMsg msg);
void receive3(CanRxMsg msg);

void determineZone(void);

void sumColour(void);

void process_array(void);

void print_data(void);

void fill_sensorbar_array(void);

void systemInit(void);

void goNormal(void);

void goUsingImu(void);

void goStraightLittleBit(void);



#endif




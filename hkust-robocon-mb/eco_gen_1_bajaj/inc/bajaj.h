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


//CHOOSE YOUR CAR
#define BAJAJ

#ifdef WILSON
#define SERVO_MICROS_MID 1150
#define SERVO_MICROS_RIGHT 550
#define SERVO_MICROS_LEFT 1750
#endif

#ifdef HITLER
#define SERVO_MICROS_MID 1500
#define SERVO_MICROS_RIGHT 2100
#define SERVO_MICROS_LEFT 1000
#define HITLER_SERVO SERVO2
#endif

#ifdef BAJAJ
#define SERVO_MICROS_MID 1300
#define SERVO_MICROS_RIGHT 2000
#define SERVO_MICROS_LEFT 600
#define BAJAJ_SERVO  SERVO3
#endif




enum{NOT_RIVER = 0, STAGE1 = 1, STAGE2 = 2, STAGE3 = 3};

void receive(CanRxMsg msg);
void receive2(CanRxMsg msg);
void receive3(CanRxMsg msg);

void process_array(void);

void print_data(void);
void fill_sensorbar_array(void);
void systemInit(void);

void goNormal(void);

void goFindWall(void);

void goStraightYolo(void);

#endif




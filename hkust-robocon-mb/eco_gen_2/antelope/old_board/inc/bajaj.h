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
#include "adc.h"
#include "linear_ccd.h"
//CHOOSE YOUR CAR


//Default Defines
#define SERVO_MICROS_MID 1350
#define SERVO_MICROS_RIGHT 900
#define SERVO_MICROS_LEFT 1800
#define BAJAJ_SERVO SERVO2

//Choosing the sides: LEFT OR RIGHT
#define RIGHT

#ifdef LEFT
#define MAX_NINETY_TURNING 800
#define IMU_ANGLE -115
#define LESSER_TURNING 225
#define RIVER_INFRARED INFRARED_SENSOR_RIGHT
#define SLOPE_ENCODER 50000
#endif

#ifdef RIGHT
#define MAX_NINETY_TURNING 1850
#define IMU_ANGLE 110
#define LESSER_TURNING -230
#define RIVER_INFRARED INFRARED_SENSOR_LEFT
#define SLOPE_ENCODER 47000
#endif



enum{NOT_RIVER = 0, STAGE1 = 1, STAGE2 = 2, STAGE3 = 3};

typedef enum{
    PINKZONE = 0, DARKGREENZONE = 1,ORANGEZONE = 2, BLUEZONE = 3, LIGHTGREENZONE = 4, NOCOLOURZONE = 5
}ZONE;

enum{OFF = 0 , ON = 1};

void receive(CanRxMsg msg);
void receive2(CanRxMsg msg);
void receive3(CanRxMsg msg);

void update_encoder(void);

void process_array(void);

void print_data(void);
void fill_sensorbar_array(void);
void systemInit(void);

void goNormal(void);

void goUsingImu(void);

void goStraightLittleBit(void);

void printSystemOff(void);

void determineZone(void);

void runUserInterface(void);


#endif




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
#define SERVO_MICROS_RIGHT 850
#define SERVO_MICROS_LEFT 1850
#define BAJAJ_SERVO SERVO2

//Choosing the sides: LEFT OR RIGHT
#define RIGHT

#ifdef LEFT
#define MAX_NINETY_TURNING 785
#define IMU_ANGLE -120
#define LESSER_TURNING 200
#define RIVER_INFRARED INFRARED_SENSOR_RIGHT

//COLOR DEFINES
#define DARKGREENLEFT 180
#define DARKGREENRIGHT 220
#define ORANGELEFT 15
#define ORANGERIGHT 38
#define RIVERLEFT 220
#define RIVERRIGHT 240
#define LIGHTGREENLEFT 100
#define LIGHTGREENRIGHT 180
#define PINKLEFT 38
#define PINKRIGHT 100
#endif

#ifdef RIGHT
#define MAX_NINETY_TURNING 1815
#define IMU_ANGLE 100
#define LESSER_TURNING -250
#define RIVER_INFRARED INFRARED_SENSOR_LEFT

//COLOR DEFINES
#define DARKGREENLEFT 180
#define DARKGREENRIGHT 220
#define ORANGELEFT 15
#define ORANGERIGHT 75
#define RIVERLEFT 220
#define RIVERRIGHT 235
#define LIGHTGREENLEFT 100
#define LIGHTGREENRIGHT 180
#define PINKLEFT 235
#define PINKRIGHT 270
#endif



enum{NOT_RIVER = 0, STAGE1 = 1, STAGE2 = 2, STAGE3 = 3};

typedef enum{
    ORANGEZONE = 0, DARKGREENZONE = 1, BLUEZONE = 2, LIGHTGREENZONE = 3, PINKZONE = 4, NOCOLOURZONE = 5
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


#endif




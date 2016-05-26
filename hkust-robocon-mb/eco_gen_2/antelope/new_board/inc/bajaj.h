#ifndef __BAJAJ
#define __BAJAJ


//Includes (Please do not change these)
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
#include "buzzer.h"

//Function prototypes and enumerations(Please do not change these)
enum{NOT_RIVER = 0, NINETY = 1, STAGE1 = 2, STAGE2 = 3, STAGE3 = 4};

enum{LEFT_SIDE = 5, RIGHT_SIDE = 6};


//Default Defines (Please do not change these)
#define SERVO_MICROS_MID 1350
#define SERVO_MICROS_RIGHT 850
#define SERVO_MICROS_LEFT 1850
#define BAJAJ_SERVO SERVO1




/*
* EXPLANATION:
* YOU JUST NEED TO CHANGE THE CONSTANTS BELOW TO CONTROL THE BEHAVIOUR OF THE ECO ROBOT.
* 1st Param: LEFT / RIGHT: Choose LEFT for RED side, Vice versa for Blue side.
* 2nd Param: MAX_NINETY_TURNING: How large do you want the turning be? 900: Right-most, 1800: Left-most.
* 3rd Param: IMU_ANGLE: Determines the angle you want to fix in the river part.
* 4th Param: LESSER_TURNING: Determines the offset angle you want to re-align the eco with the white line right after crossing the river part
* 5th Param: RIVER_INFRARED: Determines which Infrared sensor you want to use, value: INFRARED_SENSOR_RIGHT / INFRARED_SENSOR_LEFT.
* 6th Param: SLOPE_ENCODER: How much you want the encoder to turn.
* 7th Param: DELAY: How long you want the eco to retain servo angle during the 90-degree turn: Value: in milli-second.
*/

//You can change this
#define RIGHT

//You can change these
#ifdef LEFT
#define MAX_NINETY_TURNING 1000
#define IMU_ANGLE -115
#define NINETY_IMU -180
#define LESSER_TURNING 260
#define RIVER_INFRARED INFRARED_SENSOR_RIGHT
#define SLOPE_TURNING_RIGHT 1000
#define SLOPE_TURNING_LEFT 1700
#define SLOPE_ENCODER 46000
#define DELAY 1000
#define CURRENT_SIDE LEFT_SIDE
#endif

//You can change these
#ifdef RIGHT
#define MAX_NINETY_TURNING 1875
#define IMU_ANGLE 125
#define LESSER_TURNING -160
#define NINETY_IMU 160
#define RIVER_INFRARED INFRARED_SENSOR_LEFT
#define SLOPE_ENCODER 40000
#define SLOPE_TURNING_RIGHT 1000
#define SLOPE_TURNING_LEFT 1700
#define DELAY 1000
#define CURRENT_SIDE RIGHT_SIDE
#endif

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

void goNinety(void);

void goUsingImu(void);

void goStraightLittleBit(void);

void printSystemOff(void);

void determineZone(void);

void runUserInterface(void);


#endif




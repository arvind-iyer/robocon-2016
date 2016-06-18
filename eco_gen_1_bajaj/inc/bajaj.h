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
#include "adc.h"
#include "linear_ccd.h"



//Default Defines (Please do not change these)
#define SERVO_MICROS_MID 1500
#define SERVO_MICROS_RIGHT 2100
#define SERVO_MICROS_LEFT 900
#define BAJAJ_SERVO SERVO2




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
#define LEFT


//You can change these
#ifdef LEFT
#define MAX_NINETY_TURNING 2200
#define IMU_ANGLE -135
#define LESSER_TURNING 200
#define RIVER_INFRARED INFRARED_SENSOR_RIGHT
#define SLOPE_ENCODER 52000
#define DELAY 1000
#endif

//You can change these
#ifdef RIGHT
#define MAX_NINETY_TURNING 1875
#define IMU_ANGLE 105
#define LESSER_TURNING -250
#define RIVER_INFRARED INFRARED_SENSOR_LEFT
#define SLOPE_ENCODER 49000
#define DELAY 1000
#endif



//Function prototypes and enumerations(Please do not change these)
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




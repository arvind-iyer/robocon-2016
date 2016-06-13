#ifndef __BAJAJ
#define __BAJAJ

/*
*  Robocon 2016: Eco Robot (Team Fiery Dragon)
*  Files: bajaj.c & bajaj.h
*  Author: Budi Ryan (bryanaa@connect.ust.hk)
*  These files contain all the functions that control the behaviour of the robot
*/


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
#include "string.h"
#include "MTi-1_UART.h"
#include "adc.h"
#include "adc_ir.h"


//Default Defines (Please do not change these)

//These defines control the maximum left / right turning of th servo (in PWM values)
#define SERVO_MICROS_MID 1450
#define SERVO_MICROS_RIGHT 1850
#define SERVO_MICROS_LEFT 1050
#define BAJAJ_SERVO SERVO1


//STAGE1: RIVERING
//STAGE2: EXIT RIVER
//STAGE3: ENTERING RIVER
//STAGE4: DOWN SLOPE
//STAGE5: FINISH

//Function enumerations(Please do not change these)
enum{NORMAL = 0, NINETY = 1, RIVERING = 2, EXIT_RIVER = 3, ENTER_RIVER = 4 , DOWN_SLOPE = 5, FINISH = 6, RIVER2 = 7};

enum{LEFT_SIDE = 5, RIGHT_SIDE = 6};

typedef enum{
    PINKZONE = 0, LIGHTBLUEZONE, DARKGREENZONE, ORANGEZONE, BLUEZONE, LIGHTGREENZONE, NOCOLOURZONE
}ZONE;

typedef enum{
   REDSIDE = 0 , BLUESIDE = 1 
}GAMESIDE;

typedef enum{
    LEFT = 0 , RIGHT = 1
}INFRARED;

typedef enum{
    STARTZONE = 0, GREENSLOPE1, ORANGE1, GREENSLOPE2, ORANGE2, GREENSLOPE3, FINISHEDSLOPE
}SLOPEZONE;

enum{OFF = 0 , ON = 1};

//Function prototypes


/*
* desc: Controls the constant which determine the behaviour of the robot
* param: void
* retval: void
*/
void initializeValues(void);

/*
* desc: Receives 8 bytes sensorbar array from the sensorbar program
* param: CAN message
* retval: void
*/
void receive(CanRxMsg msg);

/*
* desc: Receives another 8 bytes sensorbar array from the sensorbar program
* param:  CAN message
* retval: void
*/
void receive2(CanRxMsg msg);

/*
* desc: Receives additional information from the sensorbar for determining the zone
* param: CAN message
* retval: void
*/
void receive3(CanRxMsg msg);

/*
* desc: Process the 16 bytes array received from the sensorbar and filter it
* param: void
* retval: void
*/
void process_array(void);


/*
* desc: Prints information when the servo is active
* param: void
* retval: void
*/
void print_data(void);

/*
* desc: Prints information when the servo is off
* param: void
* retval: void
*/
void printSystemOff(void);

/*
* desc: fill the array used in the main program from the array which imports the array from sensorbar program
* param: void
* retval: void
*/
void fill_sensorbar_array(void);

/*
* desc: Initialize all hardwares system UART ENCODER ETC
* param: void
* retval: void
*/
void systemInit(void);

/*
* desc: Use sensorbar to track the white line using adaptive angle
* param: void
* retval: void
*/
void goNormal(void);

/*
* desc: turn ninety degree using IMU
* param: void
* retval: void
*/
void goNinety(void);

/*
* desc: go the river portion using angle locking of IMU
* param: void
* retval: void
*/
void goUsingImu(void);

/*
* desc: Adjust the position of the robot right before leaving the river part and go using normal method again
* param: void
* retval: void
*/
void goStraightLittleBit(void);

/*
* desc: Gives the color which the sensorbar is on right now
* param: void
* retval: void
*/
void determineZone(void);

/*
* desc: function that handles button pressing handling
* param: void
* retval: void
*/
void runUserInterface(void);

/*
* desc: based on the velocity of the encoder, determine the IMU angle for turning
* param: void
* retval: angle of the IMU in float
*/
float determine_imu_angle(void);

/*
* @desc: Returns the central position of the line as read by the sensor bar
* @param: void
* @retval: int, ranges from -8 to +8 where 0 implies line is at the center of the bar -8 is left most and 8 is right
*/
int getLineCenter(void);
 
void goRiver(void);
void goRiver2(void);
void goScanRiver(void); 
void goScanRiver2(void);
#endif




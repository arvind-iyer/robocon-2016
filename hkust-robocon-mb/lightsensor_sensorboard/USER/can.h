/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _CAN_H
#define _CAN_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f10x.h"
#include "stm32f10x_can.h"
#include "misc.h"
//#include "Robocon_config.h"
#include "can_protocol.h"
/**********************************************************************
*				Device Selection
*Description: Selecting the type of device by definition
*Explanation:
*Currently we have 5 types of devices: 
*1. main control 
*2. position system
*3. motor control
*4. sensor board
*5. sensor bar
*Select the target device by commenting the rest.
***********************************************************************/
//#define MAIN_CONTROL
//#define POSITION_SYSTEM
//#define MOTOR_CONTROL
//#define SENSOR_BORAD
#define SENSOR_BAR	

//Motors Board ID
#define MOTORBOARD_A						0x0B0
#define MOTORBOARD_B						0x0B1
#define MOTORBOARD_C						0x0B2
#define MOTORBOARD_D						0x0B3
#define MOTORBOARD_E						0x0B4

#define SEN_BAR1						0x0C1
#define SEN_BAR2						0x0C2
#define SEN_BAR3						0x0C3
#define SEN_BAR4						0x0C4
#define LIGHTSENSORBAR_ID 			0x0C5


/* Constants -----------------------------------------------------------------*/
#define CAN_QUEUE_LENGTH 256
#define CAN_QUEUE_EMPTY 0

void CAN_Configuration(void);
void test_tx(void);
void CAN_addToQueue(CanRxMsg RxMsg);
u8 CAN_dequeue(void);
void CAN_messageProcessing(CanRxMsg RxMsg);
extern u8 can_rx_data;
extern CanRxMsg CAN_queueHead;
extern s32 CAN_QueueCounter;
extern CanRxMsg CAN_Queue[CAN_QUEUE_LENGTH];

/* Test ----------------------------------------------------------------------*/
void Main_TX(void);
void Device1_TX(void);
void Device2_TX(void);
void Device3_TX(void);
void Device4_TX(void);

/* Test Devices' IDs --------------------------------------------------------------*/
#define DEVICE_ONE 		0x321
#define DEVICE_TWO 		0x322
#define DEVICE_THREE 	0x323
#define DEVICE_FOUR 	0x324

/* Devices ID definition --------------------------------*/

//Gyroscope(Position system)
#define POS_SYSTEM 					0x0A0


/* Devices' data ---------------------------*/
extern u8 Device1_data;
extern u8 Device2_data;
extern u8 Device3_data;
extern u8 Device4_data;

#endif

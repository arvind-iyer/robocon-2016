#ifndef _CAN_PROTOCOL_H
#define _CAN_PROTOCOL_H

#include "can_protocol.h"
#include "stm32f10x.h"


//#define GYRO_POSITION_FEEDBACK 		0	
//#define GYRO_VELOCITY_FEEDBACK		0

#define CAN_QUEUE_EMPTY 			0
#define TX_QUEUE_LENGTH				128
#define RX_QUEUE_LENGTH				128

#define RETRY_TIMEOUT				1000

/* CONSTANTS FOR PROTOCOL ***************************************/


// Here we mean 15 motors on single board. Now we only have 3 on one board.
/* main control -> motor control */
#define MOTOR_VEL_CMD_LENGTH 		6
#define MOTOR_0_VEL_CMD 				0xA0
#define MOTOR_1_VEL_CMD 				0xA1
#define MOTOR_2_VEL_CMD 				0xA2
// after 3 = no use for now
#define MOTOR_3_VEL_CMD 				0xA3
#define MOTOR_4_VEL_CMD 				0xA4
#define MOTOR_5_VEL_CMD 				0xA5
#define MOTOR_6_VEL_CMD 				0xA6
#define MOTOR_7_VEL_CMD 				0xA7
#define MOTOR_8_VEL_CMD 				0xA8
#define MOTOR_9_VEL_CMD 				0xA9
#define MOTOR_10_VEL_CMD 				0xAA
#define MOTOR_11_VEL_CMD 				0xAB
#define MOTOR_12_VEL_CMD 				0xAC
#define MOTOR_13_VEL_CMD 				0xAD
#define MOTOR_14_VEL_CMD 				0xAE
#define MOTOR_15_VEL_CMD 				0xAF

#define MOTOR_POS_CMD_LENGTH 		8
#define MOTOR_0_POS_CMD 				0xB0
#define MOTOR_1_POS_CMD 				0xB1
#define MOTOR_2_POS_CMD 				0xB2
#define MOTOR_3_POS_CMD 				0xB3
#define MOTOR_4_POS_CMD 				0xB4
#define MOTOR_5_POS_CMD 				0xB5
#define MOTOR_6_POS_CMD 				0xB6
#define MOTOR_7_POS_CMD 				0xB7
#define MOTOR_8_POS_CMD 				0xB8
#define MOTOR_9_POS_CMD 				0xB9
#define MOTOR_10_POS_CMD 				0xBA
#define MOTOR_11_POS_CMD 				0xBB
#define MOTOR_12_POS_CMD 				0xBC
#define MOTOR_13_POS_CMD 				0xBD
#define MOTOR_14_POS_CMD 				0xBE
#define MOTOR_15_POS_CMD 				0xBF


#define MOTOR_PARAMETER_CMD_LENGTH	3
#define MOTOR_PARAMETER_CMD			0x44

#define MOTOR_LOCK_CMD_LENGTH	   	1
#define MOTOR_LOCK_CMD				0xEE	

/* main control -> gyro */
#define GYRO_CAL_CMD_LENGTH			1
#define GYRO_CAL_CMD				0xDD

#define GYRO_SET_CMD_LENGTH			7
#define GYRO_SET_CMD				0xEE

/* motor control -> main control */
#define ENCODER_FEEDBACK_LENGTH		5
#define ENCODER_0_FEEDBACK			0x22
#define ENCODER_1_FEEDBACK			0x23
#define ENCODER_2_FEEDBACK			0x24

#define MOTOR_CAL_FEEDBACK_LENGTH	1
#define MOTOR_CAL_FEEDBACK			0xEA

#define MOTOR_ID_OFFSET				0x0B0
#define NUM_OF_MOTORS               16

#define DATA1_FEEDBACK_LENGTH  		5
#define DATA1_FEEDBACK			   	0x55

#define DATA2_FEEDBACK_LENGTH	  	5
#define DATA2_FEEDBACK				0x66

#define DATA3_FEEDBACK_LENGTH		5
#define DATA3_FEEDBACK				0x77

#define DATA4_FEEDBACK_LENGTH		5
#define DATA4_FEEDBACK				0x88

/* gyro -> main control */
#define POS_FEEDBACK_LENGTH			7	
#define POS_FEEDBACK				0x11

#define VEL_FEEDBACK_LENGTH			7	
#define VEL_FEEDBACK				0x33

#define LSB_TOGGLE_CMD 0x45
#define LSB_CALIBRATE_CMD 0x46
/* Public variables ------------------------------------------*/

extern u8 Tx_QueueCounter;
extern u8 Rx_QueueCounter;


/* Struct definition ------------------------------------------*/
typedef struct
{
	u8 Data_Length;
	u8 Data[8];									//8 is the max data length of a can data frame
} Data_Field;

/* Functions declaration --------------------------------*/
s32 Four_Bytes_Reconstruction(const uint8_t* buffer);
s16 Two_Bytes_Reconstruction(const uint8_t* buffer);
u8 Four_Bytes_Decomposition(const s32 data, const u8 index);
u8 Two_Bytes_Decomposition(const u16 data, const u8 index);
u16 array_to_u16(u8 *);

void CAN_Tx_addToQueue(CanTxMsg TxMessage);
u8 CAN_Tx_dequeue(void);

void CAN_Rx_addToQueue(CanRxMsg RxMessage);
void CAN_Rx_Processing(CanRxMsg RxMessage);
u8 CAN_Rx_dequeue(void);

void CAN_Tx_update(void);
void CAN_Rx_update(void);

void USB_LP_CAN1_RX0_IRQHandler(void);

u8 Enabled(void);
CanTxMsg Sensor_Bar_Encoding(void);
void Sensor_Bar_Update(void);
CanTxMsg Light_Sensor_Bar_Encoding();
#endif




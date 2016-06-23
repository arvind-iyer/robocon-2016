#ifndef _CAN_MOTOR_H
#define _CAN_MOTOR_H

#include "can.h"
#include "can_protocol.h"

#define CAN_MOTOR_COUNT										16
#define	CAN_MOTOR_BASE										0x0B0

/*** TX ***/
#define	CAN_MOTOR_VEL_LENGTH							6
#define CAN_MOTOR_VEL_CMD									0xAA

#define CAN_MOTOR_LOCK_LENGTH	 		  	1
#define CAN_MOTOR_LOCK_CMD						0xEE	

/*** RX ***/
#define CAN_ENCODER_FEEDBACK_LENGTH				5
#define CAN_ENCODER_FEEDBACK							0x22


typedef enum {
	MOTOR1 = 0,
	MOTOR2 = 1,
	MOTOR3 = 2,
	MOTOR4 = 3,
	MOTOR5 = 4,
	MOTOR6 = 5,
	MOTOR7,
	MOTOR8,
	MOTOR9,
	MOTOR10,
	MOTOR11,
	MOTOR12,
	MOTOR13,
	MOTOR14,
	MOTOR15,
	MOTOR16
} MOTOR_ID;


typedef enum {
	OPEN_LOOP = 0,
	CLOSE_LOOP = 1
} CLOSE_LOOP_FLAG;

void can_motor_init(void);
void motor_set_vel(MOTOR_ID motor_id, s32 vel, CLOSE_LOOP_FLAG close_loop_flag);
void motor_lock(MOTOR_ID motor_id);
s32 get_encoder_value(MOTOR_ID motor_id);

#endif
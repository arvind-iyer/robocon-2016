#ifndef __GYRO_H
#define __GYRO_H

#include "tm_stm32f4_usart.h"
#include "ticks.h"
#include "tm_stm32f4_delay.h"
#include "approx_math.h"
#include "lcd.h"

#define MENU_ADD_GYRO_TEST menu_add(5, "Position Test", position_test)

#define GYRO_UART  USART3

/*** Command List ***/
#define GYRO_WAKEUP				0x01

#define GYRO_UPDATE				0x10
#define GYRO_CAL				0x20
#define GYRO_POS_SET			0x30
#define GYRO_AUTO_UPDATE		0x40

/*** Reply Command List ***/
#define GYRO_REPLY				0x50
#define GYRO_UPDATED			0x80

#define GYRO_COMMAND_LENGTH		2

#define GYRO_FLAG_SET_POS		0x01
#define GYRO_FLAG_CAL			0x02

extern volatile u8 gyro_available;
extern s16 SHIFT_X, SHIFT_Y;

s16 get_X(void);	//get x-coordinate
s16 get_Y(void);	//get y-coordinate
s16 get_angle(void);	////get angle

void gyro_init(void);
void gyro_pos_update(void);		//update gyro values only when auto-update function is disabled in gyro
u8 gyro_cal(void);	//callibrate gyro
u8 gyro_pos_set(s16 x, s16 y, s16 a);	//set position of gyro



#endif		/* __GYRO_H */
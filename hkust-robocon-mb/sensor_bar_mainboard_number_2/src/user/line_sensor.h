#ifndef _LINE_SENSOR_H
#define _LINE_SENSOR_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

#define LINE_SENSOR_1_PIN			GPIO_Pin_0
#define LINE_SENSOR_2_PIN			GPIO_Pin_1
#define LINE_SENSOR_GPIO			GPIOE

void lineSensorInit(void);

typedef enum{
	LINE_SENSOR_1 = 0,
	LINE_SENSOR_2 = 1,
} LINE_SENSOR;

//Left Digit = Left sensor
typedef enum{
	trackTurnLeft90 = 1, 			//01
	trackTurnRight90 = 2, 			//10
} TRACK_LINE_STATE;

u8 readLineSensor(LINE_SENSOR);
u8 getLineSensorAll();

#endif
#ifndef __INFRARED_SENSOR
#define __INFRARED_SENSOR

#include "tm_stm32f4_gpio.h"

#define INFRARED_SENSOR_GPIO GPIOE
#define INFRARED_1_PIN GPIO_Pin_10
#define INFRARED_2_PIN GPIO_Pin_11

typedef enum{
	INFRARED_SENSOR_1 = 0,
	INFRARED_SENSOR_2 = 1
} INFRARED_SENSOR;


void infrared_sensor_init(void);
u8 read_infrared_sensor(INFRARED_SENSOR sensor);




#endif
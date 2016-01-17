#include "stm32f10x_gpio.h"
#define SENSOR_L GPIO_Pin_9
#define SENSOR_M GPIO_Pin_10
#define SENSOR_R GPIO_Pin_12

void lineSensor_init(void);

u8 readLineSensor(u16 gpio_pin);
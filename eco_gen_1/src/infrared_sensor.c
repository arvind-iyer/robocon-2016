#include "infrared_sensor.h"


uint16_t infrared_gpio_pins[2] = {INFRARED_1_PIN, INFRARED_2_PIN};


/**
* Init function
* @param void
* @return void
**/
void infrared_sensor_init(){
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin = INFRARED_1_PIN|INFRARED_2_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(INFRARED_SENSOR_GPIO, &GPIO_InitStructure);
}

/**
* Return the state of the line sensor
* @param sensor: INFRARED_SENSOR_1 / INFRARED_SENSOR_2
* @return 0 if white, 1 if not
**/
u8 read_infrared_sensor(INFRARED_SENSOR sensor){
    return GPIO_ReadInputDataBit(INFRARED_SENSOR_GPIO, infrared_gpio_pins[sensor]);
}

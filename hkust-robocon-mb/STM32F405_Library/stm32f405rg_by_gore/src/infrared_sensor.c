#include "infrared_sensor.h"


uint16_t infrared_gpio_pins[4] = {INFRARED_LEFT_PIN, INFRARED_RIGHT_PIN, INFRARED_UPPER_LEFT_PIN, INFRARED_UPPER_RIGHT_PIN};


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
	GPIO_InitStructure.GPIO_Pin = INFRARED_LEFT_PIN|INFRARED_RIGHT_PIN|INFRARED_UPPER_LEFT_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(INFRARED_SENSOR_GPIO, &GPIO_InitStructure);
    
    GPIO_InitTypeDef GPIO_InitStructure1;
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure1.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure1.GPIO_Pin = INFRARED_UPPER_RIGHT_PIN;
	GPIO_InitStructure1.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(INFRARED_SENSOR_GPIO2, &GPIO_InitStructure1);
}

/**
* Return the state of the line sensor
* @param sensor: INFRARED_SENSOR_LEFT / INFRARED_SENSOR_RIGHT / INFRARED_SENSOR_UPPER_LEFT / INFRARED_SENSOR_UPPER_RIGHT
* @return 0 if white, 1 if not
**/
u8 read_infrared_sensor(INFRARED_SENSOR sensor){
    return sensor != INFRARED_SENSOR_UPPER_RIGHT ? GPIO_ReadInputDataBit(INFRARED_SENSOR_GPIO,infrared_gpio_pins[sensor]): GPIO_ReadInputDataBit(INFRARED_SENSOR_GPIO2, infrared_gpio_pins[sensor]);
}
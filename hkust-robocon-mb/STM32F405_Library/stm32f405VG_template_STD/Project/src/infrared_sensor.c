#include "infrared_sensor.h"


uint16_t infrared_gpio_pins[2] = {INFRARED_1_PIN, INFRARED_2_PIN};


/**
* Init function
* @param void
* @return void
**/
void infrared_sensor_init(){
    TM_GPIO_Init(INFRARED_SENSOR_GPIO,INFRARED_1_PIN|INFRARED_2_PIN,TM_GPIO_Mode_IN,TM_GPIO_OType_PP,TM_GPIO_PuPd_DOWN,TM_GPIO_Speed_Low);    
}

/**
* Return the state of the line sensor
* @param sensor: INFRARED_SENSOR_1 / INFRARED_SENSOR_2
* @return 0 if white, 1 if not
**/
u8 read_infrared_sensor(INFRARED_SENSOR sensor){
    return GPIO_ReadInputDataBit(INFRARED_SENSOR_GPIO, infrared_gpio_pins[sensor]);
}
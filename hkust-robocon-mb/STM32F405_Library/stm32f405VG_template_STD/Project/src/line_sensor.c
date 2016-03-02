#include "line_sensor.h"

uint16_t sensor_GPIO_Pins[3] = {LINE_SENSOR_1_PIN, LINE_SENSOR_2_PIN, LINE_SENSOR_3_PIN};



/**
* Init function
* @param void
* @return void
**/
void line_sensor_init(void){
    TM_GPIO_Init(LINE_SENSOR_GPIO,LINE_SENSOR_1_PIN|LINE_SENSOR_2_PIN|LINE_SENSOR_3_PIN,TM_GPIO_Mode_IN,TM_GPIO_OType_PP,TM_GPIO_PuPd_DOWN,TM_GPIO_Speed_Low);
}

/**
* Return the state of the line sensor
* @param sensor: LINE_SENSOR_1 / LINE_SENSOR_2 / LINE_SENSOR_3
* @return 0 if white, 1 if not
**/
u8 read_line_sensor(LINE_SENSOR sensor){
    return GPIO_ReadInputDataBit(LINE_SENSOR_GPIO, sensor_GPIO_Pins[sensor]);
}

/**
* Return all states of line sensors in binary, left digit is sensor 1
* @return three digit number 0 if white, 1 if not
**/
u8 get_line_sensor_all(){
	return ((GPIO_ReadInputDataBit(LINE_SENSOR_GPIO, sensor_GPIO_Pins[0]) << 2) +
					(GPIO_ReadInputDataBit(LINE_SENSOR_GPIO, sensor_GPIO_Pins[1]) << 1) +
					(GPIO_ReadInputDataBit(LINE_SENSOR_GPIO, sensor_GPIO_Pins[2])));
}
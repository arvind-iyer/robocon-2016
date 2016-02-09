#include "line_sensor.h"

uint16_t sensor_GPIO_Pins[3] = {LINE_SENSOR_1_PIN, LINE_SENSOR_2_PIN, LINE_SENSOR_3_PIN};

void line_sensor_init(void){
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = LINE_SENSOR_1_PIN | LINE_SENSOR_2_PIN | LINE_SENSOR_3_PIN;
	//GPIO_InitStructure.GPIO_Pin = LINE_SENSOR_2_PIN;
	
	GPIO_Init(LINE_SENSOR_GPIO, &GPIO_InitStructure);
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
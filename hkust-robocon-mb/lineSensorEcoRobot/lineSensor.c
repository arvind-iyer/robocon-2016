#include "lineSensor.h"

#define SENSOR_L GPIO_Pin_9
#define SENSOR_M GPIO_Pin_10
#define SENSOR_R GPIO_Pin_12


void lineSensor_init(void){
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = SENSOR_L|SENSOR_M|SENSOR_R;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}

u8 readLineSensor(uint16_t gpio_pin){
    return	GPIO_ReadInputDataBit(GPIOA, gpio_pin);
}
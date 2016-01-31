#include "GPIO_switch.h"

void GPIO_switch_init()
{

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_9|GPIO_Pin_10;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}



u8 read_GPIO_switch(GPIO_TypeDef* PORT,u16 gpio_pin)
{

return	GPIO_ReadInputDataBit(PORT,gpio_pin);

}

void check_switch(){//indicating switch is ok

if(read_GPIO_switch(GPIOA,GPIO_Pin_9)==0||read_GPIO_switch(GPIOA,GPIO_Pin_10)==0||read_GPIO_switch(GPIOA,GPIO_Pin_11)==0||read_GPIO_switch(GPIOA,GPIO_Pin_12)==0){
FAIL_MUSIC;
}
	
}
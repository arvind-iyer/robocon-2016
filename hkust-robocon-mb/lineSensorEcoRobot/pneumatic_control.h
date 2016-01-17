#include "stm32f10x.h"
#include "stm32f10x_gpio.h"


void pneumatic_init();
void pneumatic_control(GPIO_TypeDef* PORT,u16 gpio_pin,u8 state);
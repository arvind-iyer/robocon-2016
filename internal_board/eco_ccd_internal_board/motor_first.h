#include "stm32f10x.h"
#include "stm32f10x_gpio.h"	  
#include "stm32f10x_tim.h"




void motor_first_init(void);
void motor_first_control(u8 dir,u16 magnitude);//dir can be 1 or 0 , pwm can be 1 or 2  




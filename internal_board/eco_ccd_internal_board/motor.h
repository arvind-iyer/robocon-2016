#include "stm32f10x.h"
#include "stm32f10x_gpio.h"	  
#include "stm32f10x_tim.h"








void motor_zero_init(void);
void motor_zero_control(u8 dir,u16 magnitude);//dir can be 1 or 0 , pwm can be 1 or 2  




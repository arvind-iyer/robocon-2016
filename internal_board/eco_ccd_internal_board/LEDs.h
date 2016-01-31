#include "stm32f10x.h"

void LED_INIT(void);

void LED_blink(GPIO_TypeDef* PORT,u16 gpio_pin);
void LED_OFF(GPIO_TypeDef* PORT,u16 gpio_pin);
void LED_ON(GPIO_TypeDef* PORT,u16 gpio_pin);

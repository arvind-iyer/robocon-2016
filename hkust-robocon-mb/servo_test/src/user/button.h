#include "stm32f10x_gpio.h"

#define BUTTON1 GPIO_Pin_6
#define BUTTON2 GPIO_Pin_7

#define BUTTON_GPIO	GPIOB

void button_init(void);
u8 read_button(u8 buttonId);
void check_button(void);

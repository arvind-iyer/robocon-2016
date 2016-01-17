#include "stm32f10x_gpio.h"

#define BUTTON1 GPIO_Pin_13
#define BUTTON2 GPIO_Pin_14
#define BUTTON3 GPIO_Pin_15

#define BUTTON_GPIO	GPIOC

void button_init(void);
u8 read_button(u8 buttonId);
void check_button(void);

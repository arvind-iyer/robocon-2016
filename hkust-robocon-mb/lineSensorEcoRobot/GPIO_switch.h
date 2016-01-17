#include "stm32f10x_gpio.h"
#include "buzzer_song.h"
#include "tft_display.h"

void GPIO_switch_init(void);
u8 read_GPIO_switch(GPIO_TypeDef* PORT,u16 gpio_pin);
void check_switch(void);

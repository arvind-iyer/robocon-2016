#include "stm32f10x.h"
#include "ticks.h"
#include "lcd_red.h"
#include "adc.h"

extern u32 linear_ccd_buffer1[128];

#define clkPort GPIOA
#define clkPin GPIO_Pin_9

#define siPort GPIOB
#define siPin GPIO_Pin_0

#define AO_channel 4

void linear_ccd_init(void);
void linear_ccd_read(void);
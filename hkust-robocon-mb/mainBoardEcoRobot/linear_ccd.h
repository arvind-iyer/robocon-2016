#include "adc.h"
#include "stm32f10x.h"
#include "ticks.h"
#include "tft_display.h"




#define CLK_PORT GPIOA
#define CLK_PIN GPIO_Pin_8

#define SI1_PORT GPIOA
#define SI1_PIN GPIO_Pin_0

#define SI2_PORT GPIOB
#define SI2_PIN GPIO_Pin_0

#define AO1_channel 1
#define AO2_channel 2

extern u32 linear_ccd_buffer1[128];
extern u32 linear_ccd_buffer2[128];

void linear_ccd_init();
void linear_ccd_read();
void linear_ccd_print(void);
void linear_ccd_clear();
u32 * get_linear_ccd1();
void delay_57();
void moving_average(u32 data[]);
int mid_point();
int edge_l();
int edge_r();
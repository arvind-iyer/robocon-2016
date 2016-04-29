#include "adc.h"
#include "stm32f4xx.h"
#include "ticks.h"
#include "tft_display.h"

#define CLK_PORT GPIOE
#define CLK_PIN GPIO_Pin_9

#define SI1_PORT GPIOE
#define SI1_PIN GPIO_Pin_8

#define SI2_PORT GPIOE
#define SI2_PIN GPIO_Pin_8

#define AO1_channel 1
#define AO2_channel 2

//AO1: PC4/ADC12_IN14
//AO2: PC5/ADC12_IN15
//CLK: PE9/TIM1_CH1
void linear_ccd_init();
void linear_ccd_read();
void linear_ccd_print(void);
void linear_ccd_clear();
void delay_57();
void moving_average(u32 data[]);
int mid_point();
int edge_l();
int edge_r();


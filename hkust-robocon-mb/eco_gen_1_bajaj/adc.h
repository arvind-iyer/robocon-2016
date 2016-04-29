#ifndef __ADC_H
#define __ADC_H

#include "stm32f4xx.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dma.h"
#define ADC_NUM ADC1
#define ADC_PERIPH RCC_APB2Periph_ADC1
#define ADC_GPIO GPIOC
#define ADC_GPIO_PORT_1 GPIO_Pin_4
#define ADC_GPIO_PORT_2 GPIO_Pin_5
#define ADC_CHANNEL_1 ADC_Channel_14
#define ADC_CHANNEL_2 ADC_Channel_15


void adc_init(void);
u32 get_adc(u8 i);

#endif  /** __ADC_H **/

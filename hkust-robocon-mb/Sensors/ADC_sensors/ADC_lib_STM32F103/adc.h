#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"

#define ADC_GPIO_RCC	(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA)
#define ADC_Device		2

//Under port C
#define ADC_1_Pin 		GPIO_Pin_3
#define ADC_2_Pin 		GPIO_Pin_2
#define ADC_3_Pin 		GPIO_Pin_1
#define	ADC_8_Pin			GPIO_Pin_4
#define ADC_9_Pin			GPIO_Pin_5

//Under port A
#define ADC_4_Pin			GPIO_Pin_4
#define ADC_5_Pin			GPIO_Pin_5
#define ADC_6_Pin			GPIO_Pin_6
#define ADC_7_Pin			GPIO_Pin_7

typedef struct
{
	GPIO_TypeDef*		GPIO;
	uint16_t				ADC_Pin;
	uint8_t					ADC_Channel;
	uint8_t					priority;
	uint8_t					sample_time;
}	ADC_structure;

void adc_init(void);
u32 get_adc(u8 i);

#endif  /** __ADC_H **/

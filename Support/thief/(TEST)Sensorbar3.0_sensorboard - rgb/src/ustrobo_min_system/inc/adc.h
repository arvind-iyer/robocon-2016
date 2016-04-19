#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"


void adc_init(void);
u32 get_adc(u8 i);

#endif  /** __ADC_H **/

#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdio.h>
#include "stm32f10x.h"
#include "sysinit.h"
#include "misc.h"
#include "stm32f10x_rcc.h"

#include "main.h"
#include "motion.h"

void system_init(u16 sysFreq);
void system_pwm_enable(void);
void system_para_init(void);
//void system_shake_hand(u8 mode);

#define ADC1_DR_Address    ((u32)0x4001244C)


#endif

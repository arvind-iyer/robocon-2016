#ifndef __TICKS_H
#define __TICKS_H
#include "buzzer_song.h"
#include "stm32f10x_tim.h" 

#define TICKS_TIM							TIM2
#define TICKS_RCC							RCC_APB1Periph_TIM2
#define TICKS_IRQn						TIM2_IRQn
#define TICKS_IRQHandler			void TIM2_IRQHandler(void)

//#define	TICKS_IRQHandler				void SysTick_Handler(void)
extern volatile u32 ticks;

u32 get_us_ticks(void);
u16 get_ms_ticks(void);
u16 get_seconds(void);
u32 get_real_us(void);
u32 get_full_ticks(void);
u32 get_real_ticks(void);
void ticks_init(void);
void playSong(const MUSIC_NOTE_DUR* song);

void _delay_us(u32 nus);
void _delay_ms(u16 nms);

void simple_delay1_ms(void);
void simple_delay10_us(void);
#endif		/*  __TICKS_H */

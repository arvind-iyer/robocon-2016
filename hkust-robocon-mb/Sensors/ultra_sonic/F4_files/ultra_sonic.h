#ifndef __ULTRA_SONIC_H
#define __ULTRA_SONIC_H

#include "tm_stm32f4_gpio.h"
#include "tm_stm32f4_delay.h"
#include "led.h"

#define US_TIM 	TIM10
#define US_RCC	RCC_APB2Periph_TIM10
#define US_IRQn	TIM1_UP_TIM10_IRQn
#define US_IRQHandler	void TIM1_UP_TIM10_IRQHandler()

#define US_TRIG_PULSE         	6
    // 10 us
//#define US_ECHO_PULSE_COUNT   5
//#define US_MAX_WIDTH					1000	// 3400mm

//#define	US_IDLE_RESET_COUNT		60000		// 12 ms
//#define	US_TAKE_TURN_BREAK		100

#define CLK											72000000
#define	US_RESET_TIME						30000	// 30ms
#define	US_DEVICE_COUNT					8

#define US_PORT 	GPIOA
#define TRIG_PIN_1 	7
#define ECHO_PIN_1 	6

#define US_COUNT		2

typedef enum {
	US_NULL								= 0,
	US_READY							= 1,
	US_TRIGGER 						= 2,
	US_WAITING_FOR_ECHO		= 3,
	US_ECHO_RAISED				= 4,
	US_ECHO_FALLEN				= 5
	
} US_STATE;

typedef struct {
	//const GPIO* trig_gpio, *echo_gpio;
	US_STATE state;
	u32 pulse_width_tmp, pulse_width;
	volatile u16 trigger_time_us, falling_time_us;
} US_TypeDef;

typedef enum {
	US_INDEPENDENT,
	US_SYNC,
	US_TAKE_TURN
} US_MODE;

typedef struct {
	u32 in_range_full_ticks, out_range_full_ticks;
	u16 in_distance; 
	u16 in_range_time;
} US_PROC_TypeDef;

void us_init();
US_MODE us_get_mode(void);
US_STATE us_get_state(u8 i);
u32 us_get_pulse_raw(u8 i);
u32 us_get_pulse(u8 i);
u32 us_get_distance(u8 i);
u8 us_get_speed(void);
u8 us_get_current_us(void);
void us_proc_init(void);
void us_proc_update(void);
u8 us_in_range(u8 i);

#endif
#include "cmsis_os.h"
#include "stm32f4xx_hal.h" 


void delay_ms(uint32_t time);

//TICKS
#ifdef RTE_CMSIS_RTOS_RTX
extern uint32_t os_time;

uint32_t get_ms_ticks(void);
#endif
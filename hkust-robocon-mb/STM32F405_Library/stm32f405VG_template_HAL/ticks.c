#include "ticks.h"

void delay_ms(uint32_t time){
    osDelay(time);
}

uint32_t get_ms_ticks(void) { 
  return os_time; 
}
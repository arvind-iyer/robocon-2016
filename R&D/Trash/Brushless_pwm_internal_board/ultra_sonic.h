#include "ticks.h"
#include "stm32f10x.h"

#define TRIG_PORT GPIOA
#define TRIG_PIN  GPIO_Pin_11
#define ECHO_PORT GPIOA
#define ECHO_PIN  GPIO_Pin_12

void ultra_sonic_init();
u32 call_ultra_sonic();
#include "stm32f10x_gpio.h"
#include "ticks.h"

#define commu_port GPIOA
#define commu_tx_pin1 GPIO_Pin_13
#define commu_tx_pin2 GPIO_Pin_14 
#define commu_rx_pin1 GPIO_Pin_11
#define commu_rx_pin2 GPIO_Pin_12

void commu_tx_init();
void commu_rx_init();
void data_tx(u8,  uint16_t);
u8 data_rx(uint16_t);
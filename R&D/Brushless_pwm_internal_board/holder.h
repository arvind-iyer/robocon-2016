#include "stm32f10x.h"

#define detect_port GPIOA
#define left_pin GPIO_Pin_9
#define right_pin GPIO_Pin_10

//void slow_line_track();
void tubes_check_init();
u8 distance_checking();
//u8 tubes_checking(uint16_t);

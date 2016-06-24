#ifndef _COMM_H
#define _COMM_H

#include "stm32f4xx.h"
#include "can_protocol.h"
#include "ticks.h"
#include "usart.h"
#include <stdlib.h>
#include <stdbool.h>
#include "main.h"

#define SENSOR_BAR_UART_COM COM2

#define SENSOR_BAR_REQUEST_COLOR 0x0B0
#define SENSOR_BAR_AVG_COLOR_RETURN 0x0B1
#define SENSOR_BAR_WHITE_COLOR_RETURN 0x0B2
#define SENSOR_BAR_REPLACE_COLOR 0x0B3
#define SENSOR_BAR_REPLACED_CNT 0x0B4

#define UART_REPLACE_COLOR_START_FLAG 0x0A0
#define UART_REPLACE_COLOR_LENGTH 8 //1 for region, 1 for mid/back, 6 for rgb

typedef enum{
	NOT_RECEIVING_ANYTHING = 0,
	RECEIVING_REPLACEMENT_COLOR = 1
}UART_COMM_STATE;

extern u16 received_color_count;

void comm_init(void);
void comm_update(void);

#endif

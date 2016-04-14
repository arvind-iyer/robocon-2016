#ifndef _DEBUG_H
#define _DEBUG_H

#include "stm32f10x.h"
#include "bluetooth.h"
#include "uart.h"
#include "lcd_main.h"

#define RECEIVE_PACKET_START 202
#define INSTRUCTION_SET_NEW_VAR 98
#define INSTRUCTION_DATA_IN 12

void debug_init(void);
void new_graph(u8 graph_id, char* graph_name, char* xAxis, char* yAxis);
void send_data(void);

#endif
#ifndef _COMM_H
#define _COMM_H

#include "stm32f10x.h"
#include "can.h"
#include "can_protocol.h"
#include "delay.h"
#include "flashMem.h"
#include "ticks.h"
#include "uart.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include <stdlib.h>
#include <stdbool.h>
#include "main.h"

#define SENSOR_BAR_REQUEST_COLOR 0x0B0
#define SENSOR_BAR_AVG_COLOR_RETURN 0x0B1
#define SENSOR_BAR_WHITE_COLOR_RETURN 0x0B2

void comm_init(void);
void comm_update(void);

#endif

#ifndef _LASER_RANGE_FINDER_H_
#define _LASER_RANGE_FINDER_H_

#include "uart.h"
#include "lrf_string.h"

#define LRF_COMx COM3
#define LRF_Handler void USART3_IRQHandler(void)
typedef enum {
	NORMAL = 0,
	WAIT_FOR_ECHO = 1,
	ECHO_NOT_MATCH = 2,
	WAIT_FOR_STATUS = 3,
	SCIP2_NOT_SUCESS = 4,
	LASER_MALFUNCTION = 5,
	SUM_NOT_EQUAL = 6
}LRF_stat;

LRF_stat LRF_init(void);
LRF_stat LRF_send_cmd();


#endif	//_LASER_RANGE_FINDER_H_

#ifndef _CAN_H
#define _CAN_H

#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_rcc.h"
#include "misc.h"
#include "lcd.h"
#include "tm_stm32f4_gpio.h"
//PA11 RX PA12 TX

#define	CANn							CAN2
#define CAN_RCC						RCC_APB1Periph_CAN2

#define	CAN_Rx_GPIO_Pin		    GPIO_Pin_5
#define	CAN_Tx_GPIO_Pin		    GPIO_Pin_6
#define	CAN_GPIO_PORT			GPIOB
#define CAN_GPIO_RCC			RCC_AHB1Periph_GPIOB

#define CAN_TX_RETRY_TIMEOUT	800		// To prevent infinite loop

#define	CAN_RX_FILTER_LIMIT		28		// The number of filters can be applied at most

typedef enum {
	CAN_TX_FAILED 		= CAN_TxStatus_Failed,
	CAN_TX_OKAY				= CAN_TxStatus_Ok,
	CAN_TX_PENDING		= CAN_TxStatus_Pending,
	CAN_TX_NOMAILBOX	= CAN_TxStatus_NoMailBox
} CAN_TX_STATUS;

void can_init(void);
u8 can_tx(CanTxMsg msg);


#endif

#include "can.h"


/**
  * @brief  Configure and initialize the CAN (GPIOs + CAN only).
  * @param  None
  * @retval None
  */
void can_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	
	/* RCC enable */
    
	RCC_AHB1PeriphClockCmd(CAN_GPIO_RCC, ENABLE);
	RCC_APB1PeriphClockCmd(CAN_RCC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_Tx_GPIO_Pin | CAN_Rx_GPIO_Pin, GPIO_AF_CAN1);

	/* CAN GPIO init */
	// CAN_Rx Pin
	/*GPIO_InitStructure.GPIO_Pin = CAN_Rx_GPIO_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStructure);
	
	// CAN_Tx Pin
	GPIO_InitStructure.GPIO_Pin = CAN_Tx_GPIO_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStructure);*/
    
    TM_GPIO_InitAlternate(GPIOA, CAN_Tx_GPIO_Pin | CAN_Rx_GPIO_Pin, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF_CAN1);
	
	
	/* CAN register init */
	CAN_DeInit(CANn);
	CAN_StructInit(&CAN_InitStructure);
	
	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = ENABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	
	/* CAN Baudrate = 1 MBPS */
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	CAN_InitStructure.CAN_Prescaler = 6;
	CAN_Init(CANn, &CAN_InitStructure);
	
	/* CAN FIFO0 message pending interrupt enable */ 
	CAN_ITConfig(CANn, CAN_IT_TME, ENABLE);
    CAN_ITConfig(CANn, CAN_IT_FMP0, ENABLE);
	
	/* CAN TX interrupt */
	{
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel= CAN1_TX_IRQn; 
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
}


/**
	* @brief Transfer a CAN message
	* @param msg: the CAN message
	* @retval True if the message can be tranferred
	*/
u8 can_tx(CanTxMsg msg)
{
	return CAN_Transmit(CANn, &msg) != CAN_TxStatus_NoMailBox;							//transmit the message
}


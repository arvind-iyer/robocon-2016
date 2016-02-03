#include "can.h"



//
//CAN & GPIO & TIM CONFIGURATION
//
void RCC_Configuration(void) {
 /* ENABLE CLOCKS */
 /* GPIOB clock enable */
 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
 /* CAN1 clock enable */
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
}

void GPIO_Configuration(void) {
 GPIO_InitTypeDef GPIO_InitStructureCAN_RX;
 GPIO_InitTypeDef GPIO_InitStructureCAN_TX;

 /* GPIO CAN_RX Configuration */
 GPIO_InitStructureCAN_RX.GPIO_Pin = GPIO_Pin_11;
 GPIO_InitStructureCAN_RX.GPIO_Mode = GPIO_Mode_AF;
 GPIO_Init(GPIOA, &GPIO_InitStructureCAN_RX);

 /* GPIO CAN_TX Configuration */
 GPIO_InitStructureCAN_TX.GPIO_Pin = GPIO_Pin_12;
 GPIO_InitStructureCAN_TX.GPIO_Mode = GPIO_Mode_AF;
 GPIO_InitStructureCAN_TX.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructureCAN_TX.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_InitStructureCAN_TX.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOA, &GPIO_InitStructureCAN_TX);

 /* Connect CAN_RX & CAN_TX to AF9 */
 GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1); //CAN_RX = PB12
 GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1); //CAN_TX = PB13
}

void CAN_Configuration(void) {
 CAN_InitTypeDef CAN_InitStructure;
 RCC_ClocksTypeDef     RCC_Clocks;

 /* CAN2 reset */
 CAN_DeInit(CAN1);

 /* CAN2 configuration */
 CAN_InitStructure.CAN_TTCM = DISABLE; // time-triggered communication mode = DISABLED
 CAN_InitStructure.CAN_ABOM = ENABLE; // automatic bus-off management mode = DISABLED
 CAN_InitStructure.CAN_AWUM = DISABLE; // automatic wake-up mode = DISABLED
 CAN_InitStructure.CAN_NART = DISABLE; // non-automatic retransmission mode = DISABLED
 CAN_InitStructure.CAN_RFLM = DISABLE; // receive FIFO locked mode = DISABLED
 CAN_InitStructure.CAN_TXFP = DISABLE; // transmit FIFO priority = DISABLED
 CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; // normal CAN mode
 CAN_InitStructure.CAN_SJW = CAN_SJW_1tq; // synchronization jump width = 1
 CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq; //14
 CAN_InitStructure.CAN_BS2 = CAN_BS2_7tq; //6
 CAN_InitStructure.CAN_Prescaler = 3; // baudrate 125kbps
 //CAN_InitStructure.CAN_Prescaler = 4; // baudrate 500 kbps
 RCC_GetClocksFreq(&RCC_Clocks);
 tft_prints(0,5,"Freq: %d",RCC_Clocks.PCLK1_Frequency);
 tft_prints(0,6,"Prescaler: %d", CAN_InitStructure.CAN_Prescaler);
 CAN_Init(CAN1, &CAN_InitStructure);
}


/**
  * @brief  Configure and initialize the CAN (GPIOs + CAN only).
  * @param  None
  * @retval None
  */
void can_init(void)
{
	/* Initialize Clocks */
      RCC_Configuration();
      /* Initialize GPIO */
      GPIO_Configuration();
      /* Initialize CAN */
      CAN_Configuration();
	
	/* CAN FIFO0 message pending interrupt enable */ 
	CAN_ITConfig(CANn, CAN_IT_TME, ENABLE);
    CAN_ITConfig(CANn, CAN_IT_FMP0, ENABLE);
	
	/* CAN TX interrupt */
	{
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel= CAN1_TX_IRQn; 
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
        tft_prints(0,3,"RUN TX IRQ");
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


#include "can.h"

CanTxMsg TxMessage;
CanRxMsg RxMessage;
CanRxMsg CAN_Queue[CAN_QUEUE_LENGTH];	//CAN Msg buffer
s32 CAN_QueueCounter = 0;					//number of entries in the queue
u8 CAN_QueueHead = 0;
u8 CAN_QueueTail = 0;
CanRxMsg CAN_queueHead;
u8 can_rx_data = 0;

void CAN_GPIO_Configuration(void){
	GPIO_InitTypeDef GPIO_InitStructure;				// written inside the function so have no influence to the outside
			
	/* Configure CAN pin: RX */
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure CAN pin: TX */
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
	//GPIO_PinRemapConfig(GPIO_Remap2_CAN1, ENABLE);
}

void CAN_NVIC_Configuration(void){
	NVIC_InitTypeDef NVIC_InitStructure;

	/* enabling interrupt */
	NVIC_InitStructure.NVIC_IRQChannel=USB_LP_CAN1_RX0_IRQn; //USB低优先级或者CAN接收0中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void CAN_Configuration(void){
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	
	CAN_GPIO_Configuration();
	CAN_NVIC_Configuration();

	/* GPIO clock enable */
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	/* CAN register init */
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	
	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = ENABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	
	/* CAN Baudrate = 1mbps*/
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	CAN_InitStructure.CAN_Prescaler = 6;
	CAN_Init(CAN1, &CAN_InitStructure);

	/* CAN FIFO0 message pending interrupt enable */ 
  	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
	
	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = LIGHTSENSORBAR_ID << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0xFFFF;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
}

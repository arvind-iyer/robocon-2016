#include "stm32f10x.h"

void USART1_Configuration(void);
void USART1_SendData(u8 SendData);
extern void Delayms(vu32 m);

void USART1_Configuration()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef  USART_ClockInitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB ,ENABLE );//| RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE  );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 |RCC_APB2Periph_USART1, ENABLE  );

	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 //	ѡ�йܽ�9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		 // �����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 // ����������50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);				 // ѡ��A�˿�
    
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;			  //ѡ�йܽ�10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	  //��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);				  //ѡ��A�˿�


	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;			// ʱ�ӵ͵�ƽ�
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;				// ʱ�ӵ͵�ƽ
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;				// ʱ�ӵڶ������ؽ������ݲ���
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;		// ���һλ���ݵ�ʱ�����岻��SCLK���
	/* Configure the USART1 synchronous paramters */
	USART_ClockInit(USART1, &USART_ClockInitStructure);					// ʱ�Ӳ�����ʼ������
	
	USART_InitStructure.USART_BaudRate = 9600;						  // ������Ϊ��115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;			  // 8λ����
	USART_InitStructure.USART_StopBits = USART_StopBits_1;				  // ��֡��β����1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No ;				  // ��żʧ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// Ӳ��������ʧ��
	
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		  // ����ʹ��+����ʹ��
	/* Configure USART1 basic and asynchronous paramters */
	USART_Init(USART1, &USART_InitStructure);
    
	/* Enable USART1 */
	USART_ClearFlag(USART1, USART_IT_RXNE); 			//���жϣ�����һ�����жϺ����������ж�
	USART_ITConfig(USART1,USART_IT_RXNE, ENABLE);		//ʹ��USART1�ж�Դ
	USART_Cmd(USART1, ENABLE);							//USART1�ܿ��أ����� 
}
void  USART1_SendData(u8 SendData)
{
	USART_SendData(USART1, SendData);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
}

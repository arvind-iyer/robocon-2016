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
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 //	选中管脚9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		 // 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 // 最高输出速率50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);				 // 选择A端口
    
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;			  //选中管脚10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	  //浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);				  //选择A端口


	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;			// 时钟低电平活动
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;				// 时钟低电平
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;				// 时钟第二个边沿进行数据捕获
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;		// 最后一位数据的时钟脉冲不从SCLK输出
	/* Configure the USART1 synchronous paramters */
	USART_ClockInit(USART1, &USART_ClockInitStructure);					// 时钟参数初始化设置
	
	USART_InitStructure.USART_BaudRate = 9600;						  // 波特率为：115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;			  // 8位数据
	USART_InitStructure.USART_StopBits = USART_StopBits_1;				  // 在帧结尾传输1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No ;				  // 奇偶失能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// 硬件流控制失能
	
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		  // 发送使能+接收使能
	/* Configure USART1 basic and asynchronous paramters */
	USART_Init(USART1, &USART_InitStructure);
    
	/* Enable USART1 */
	USART_ClearFlag(USART1, USART_IT_RXNE); 			//清中断，以免一启用中断后立即产生中断
	USART_ITConfig(USART1,USART_IT_RXNE, ENABLE);		//使能USART1中断源
	USART_Cmd(USART1, ENABLE);							//USART1总开关：开启 
}
void  USART1_SendData(u8 SendData)
{
	USART_SendData(USART1, SendData);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
}

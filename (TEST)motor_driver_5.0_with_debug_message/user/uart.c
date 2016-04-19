#include "uart.h"

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

u8 printf_flag;

/**
  * @brief  Inintialization of USART
  * @param  COM: which USART to inialialize
  * @param  br: Baudrate used for USART
  * @retval None
  */
void uart_init(u32 br)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(COM1_TX_GPIO_CLK | COM1_RX_GPIO_CLK | RCC_APB2Periph_AFIO | COM1_CLK, ENABLE);

	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = COM1_TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(COM1_TX_GPIO_PORT, &GPIO_InitStructure);

	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = COM1_RX_PIN;
	GPIO_Init(COM1_RX_GPIO_PORT, &GPIO_InitStructure);

	/* USART configuration */
	USART_InitStructure.USART_BaudRate = br;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);
}

/**
  * @brief  Enable the interrupt of USART
  * @param  COM: which USART to enable interrupt
  * @retval None
  */
void uart_interrupt()
{
	NVIC_InitTypeDef NVIC_InitStructure;

	#ifdef VECT_TAB_RAM
	NVIC_SetVectorTable(NVIC_VectTab_RAM,0x0);
	#else
	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x0);
	#endif

	NVIC_InitStructure.NVIC_IRQChannel = COM1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/* Enables the USART receive interrupt */
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
}

/**
  * @brief  Enable the function of sending data of Printf via USART
  * @param  COM: which USART to be used for Printf
  * @retval None
  */
void uart_printf_enable()
{
	printf_flag = 1;
}

/**
  * @brief  Disable the function of sending data of Printf via UART
  * @param  None
  * @retval None
  */
void uart_printf_disable(void)
{
	printf_flag = 0;
}

/**
  * @brief  Sending one byte of data via USART
  * @param  COM: which USART to be used for sending data
  * @param  data: one byte data to be sent
  * @retval None
  */
void uart_tx_byte(uc8 data)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); 
	USART_SendData(USART1,data);
}

/**
  * @brief  Sending multiple bytes of data via USART
  * @param  COM: which USART to be used for sending data
  * @param  tx_buf: string to be sent
  * @retval None
  */
void uart_tx(uc8 * tx_buf, ...)
{
	va_list arglist;
	u8 buf[40], *fp;
	
	va_start(arglist, tx_buf);
	vsprintf((char*)buf, (const char*)tx_buf, arglist);
	va_end(arglist);
	
	fp = buf;
	while (*fp)
		uart_tx_byte(*fp++);
}

/**
  * @brief  Binding of function Printf
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
	if (printf_flag == 0)
		return ch;
		
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	USART_SendData(USART1, (uint8_t) ch);
	return ch;
}

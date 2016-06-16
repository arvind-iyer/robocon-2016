#include "usart.h"
#include <stdio.h>
#include <stdarg.h>
#include "buzzer.h"

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

static COM_TypeDef printf_COMx = COM_NULL;

// Backward compatible extern variable
USART_TypeDef* COM_USART[COMn] = {USART1, USART2, USART3, UART4, UART5}; 
u8 USART_QUEUE[COMn][USART_DEQUE_SIZE] = {{0}};

void uart_init(COM_TypeDef COMx, u32 baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_TYPE* usart = &USART_DEF[COMx];
	
	// Enable USART RCC Clock
	RCC_APB2PeriphClockCmd(usart->TX_RCC | usart->RX_RCC | RCC_APB2Periph_AFIO, ENABLE);
	
	switch (COMx) {
		case COM1:
			RCC_APB2PeriphClockCmd(usart->USART_RCC, ENABLE);
			break;
		case COM2:
		case COM3:
		case COM4:
		case COM5:
			RCC_APB1PeriphClockCmd(usart->USART_RCC, ENABLE);
			break;
	}
	
	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = usart->TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(usart->TX_PORT, &GPIO_InitStructure);

	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = usart->RX_PIN;
	GPIO_Init(usart->RX_PORT, &GPIO_InitStructure);

	/* USART configuration */
	USART_DeInit(usart->USART);
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(usart->USART, &USART_InitStructure);
	USART_Cmd(usart->USART, ENABLE);
	
	
	/* NVIC configuration */
	NVIC_InitStructure.NVIC_IRQChannel = usart->IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Initialize the USART receive interrupt */
	USART_ITConfig(usart->USART,USART_IT_RXNE,DISABLE);	
	USART_ITConfig(usart->USART,USART_IT_TXE,DISABLE);	
	
	/* Initialize the deque structure */
	(usart->deque).head = 0;
	(usart->deque).tail = 0;
	(usart->deque).length = USART_DEQUE_SIZE;
	(usart->deque).queue = USART_QUEUE[COMx];
	
}

void uart_printf_enable(COM_TypeDef COMx) 
{
  printf_COMx = COMx;
}

void uart_printf_disable(void)
{
  printf_COMx = COM_NULL;
}

void uart_rx_init(COM_TypeDef COMx, void (*handler)(u8 rx_data))
{
	/* Enables the USART receive interrupt */
	USART_TYPE* usart = &USART_DEF[COMx];
	USART_ITConfig(usart->USART, USART_IT_RXNE, ENABLE);	
	usart->rx_handler = handler;
}

/**
	* @brief Check if the USART_TX queue is empty
	* @param None
	* @retval True if the queue is empty
	*/
u8 uart_tx_queue_empty(COM_TypeDef COMx)
{
	USART_TYPE* usart = &USART_DEF[COMx];
	return (usart->deque).head == (usart->deque).tail;
}


/**
	* @brief	Get the current USART_TX queue size
	* @param 	None
	* @retval	The current queue size (0 to USART_TX_QUEUE_MAX_SIZE-1)
	*/
u16 uart_tx_queue_size(COM_TypeDef COMx)
{
	USART_TYPE* usart = &USART_DEF[COMx];
	s16 size = (usart->deque).tail - (usart->deque).head;
	if (size < 0) {size += (usart->deque).length;}
	return (u16) size;
}


/** 
	* @brief Add a new tx message to the usart Tx queue
	* @param msg: The usart message that will be added
	* @retval 0: Fail to enqueue due to the exceeding size, 1: Successfully enqueued
	*/
u8 uart_tx_enqueue(COM_TypeDef COMx, uc8 data){
	USART_TYPE* usart = &USART_DEF[COMx];
	USART_DEQUE* deque = &(usart->deque);

	u8 queue_full=0;
 {
		if ((deque->tail+1) % deque->length == deque->head){
			queue_full = 1;
		} else {
			deque->queue[deque->tail] = data;
			deque->tail = (deque->tail + 1) % deque->length;
			queue_full = 0;
			//USART_ITConfig(usart->USART, USART_IT_TXE, ENABLE);	
		}
	}
	//uart_tx_dequeue(COMx);
	return !queue_full;
}

/**
	* @brief	Process and transfer ONE usart message in the queue and dequeue
	* @param 	None
	*	@retval True if the queue is not empty after dequeue
	*/
u16 uart_tx_dequeue(COM_TypeDef COMx)
{
	USART_TYPE* usart = &USART_DEF[COMx];
	USART_DEQUE* deque = &(usart->deque);
	/* If USART TX is available  */
	if(USART_GetFlagStatus(USART_DEF[COMx].USART, USART_FLAG_TXE) != RESET){
		//USART_ClearFlag(usart->USART, USART_FLAG_TXE);
		if(!uart_tx_queue_empty(COMx)){  // Non-empty deque
			USART_SendData(usart->USART, deque->queue[deque->head]);
			deque->head = (deque->head + 1) % deque->length;
			USART_ITConfig(usart->USART,USART_IT_TXE,DISABLE);	
			if (uart_tx_queue_empty(COMx)) {
				buzzer_control(1, 10);
				USART_ClearITPendingBit(usart->USART, USART_IT_TXE); 
				USART_ITConfig(usart->USART,USART_IT_TXE,DISABLE);	
			}
			return 1;
		} else {
			//USART_ClearITPendingBit(usart->USART, USART_IT_TXE);  
			USART_ITConfig(usart->USART,USART_IT_TXE,DISABLE);	
			return 0;
		}
	} else {
		return 0;
	}
}

void uart_tx_byte(COM_TypeDef COMx, char data)
{
	//uart_tx_enqueue(COMx, data);
  while (USART_GetFlagStatus(USART_DEF[COMx].USART, USART_FLAG_TXE) == RESET);
	USART_SendData(USART_DEF[COMx].USART, data);
}


void uart_tx(COM_TypeDef COMx, const char * tx_buf, ...)
{
	va_list arglist;
	u8 buf[40], *fp;
	va_start(arglist, tx_buf);
	vsprintf((char*)buf, (const char*)tx_buf, arglist);
	va_end(arglist);
	
	fp = buf;
	while (*fp)
		uart_tx_byte(COMx,*fp++);
}

const USART_DEQUE* uart_get_queue(COM_TypeDef COMx)
{
	return &(USART_DEF[COMx].deque);
}



void USART_Tx_IRQHandler(COM_TypeDef COMx) 
{
	
	USART_TYPE* usartx = &USART_DEF[COMx];
	USART_TypeDef* const usart = usartx->USART;
	buzzer_control(1,10);
	if (USART_GetITStatus(usart, USART_IT_TXE) != RESET)
	{
		USART_ClearITPendingBit(usart, USART_IT_TXE);
		USART_ClearFlag(usart, USART_IT_TXE);
		
		//uart_tx_dequeue(COMx);
	}
}

void USART_Rx_IRQHandler(COM_TypeDef COMx) {
	USART_TYPE* usartx = &USART_DEF[COMx];
	USART_TypeDef* const usart = usartx->USART;
	if (USART_GetITStatus(usart, USART_IT_RXNE) != RESET) {
		USART_ClearITPendingBit(usart, USART_IT_RXNE);
		if (usartx->rx_handler != 0) {
			usartx->rx_handler(USART_ReceiveData(usart));
		}
	}
}

void USART1_IRQHandler(void) {
	USART_Rx_IRQHandler(COM1);
	USART_Tx_IRQHandler(COM1);
}

void USART2_IRQHandler(void) {
	USART_Rx_IRQHandler(COM2);
	USART_Tx_IRQHandler(COM2);
}

void USART3_IRQHandler(void) {
	USART_Rx_IRQHandler(COM3);
	USART_Tx_IRQHandler(COM3);
}

/*** Caution: 4 and 5 MUST be UART rather than USART ***/
void UART4_IRQHandler(void) {
	USART_Rx_IRQHandler(COM4);
	USART_Tx_IRQHandler(COM4);
}

void UART5_IRQHandler(void) {
	USART_Rx_IRQHandler(COM5);
	USART_Tx_IRQHandler(COM5);
}

/**
  * @brief  Binding of function Printf
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
	if (printf_COMx != COM_NULL)  {
    uart_tx_byte(printf_COMx, ch);
  }
  return ch;
}


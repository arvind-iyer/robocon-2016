#ifndef __UART_H
#define __UART_H

#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"

#include <stdio.h>
#include <stdarg.h>

// Definition for USART1 
#define COM1_CLK                    RCC_APB2Periph_USART1
#define COM1_TX_PIN                 GPIO_Pin_9
#define COM1_TX_GPIO_PORT           GPIOA
#define COM1_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define COM1_RX_PIN                 GPIO_Pin_10
#define COM1_RX_GPIO_PORT           GPIOA
#define COM1_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define COM1_IRQn                   USART1_IRQn

// COM1=USART1, COM2=USART2; ..., COM5=UART5
void uart_init(u32 br);
void uart_interrupt();

void uart_printf_enable();
void uart_printf_disable(void);

void uart_tx_byte(const uint8_t data);
void uart_tx(const uint8_t * tx_buf, ...);

#endif		/* __UART_H */

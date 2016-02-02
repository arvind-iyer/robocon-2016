/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stm32f10x_exti.h"
#include "led.h"
#include "delay.h"
extern volatile u16 ADC_val[16];
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
/*
void ADC1_2_IRQHandler(void){
	flag = TRUE;
	ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
}
*/
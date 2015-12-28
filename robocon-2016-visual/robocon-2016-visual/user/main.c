#include "main.h"

u16 ticks_last = 0;
int16_t m1, m2, m3;


void print_graph(){
	tft_prints(6, 1, "_____>");
	tft_prints(8, 2, "|");
	tft_prints(8, 3, "|");
	tft_prints(7, 0, "%d", m1);
	
	tft_prints(7, 4, "/");
	tft_prints(6, 5, "/");
	tft_prints(5, 6, "\\");
	tft_prints(3, 5, "^\\");
	tft_prints(4, 7, "%d", m2);
	
	tft_prints(9, 4, "\\");
	tft_prints(10, 5, "\\");
	tft_prints(10, 6, "</");
	tft_prints(12, 5, "/");
	tft_prints(12, 7, "%d", m3);
}

int main(void)
{
	tft_init(1, RED, BLACK, BLUE);
	can_init();
	can_rx_init();
	uart_init(COM1, 115200);
	uart_interrupt(COM1);
	led_init();
	m1 = m2 = m3 = 0;
	
	while(1){
		tft_prints(2,2,"%d",get_ticks()-ticks_last);
		if (get_ticks()-ticks_last>500){
			m1 = m2 = m3 = 0;
			tft_clear();
			print_graph();
			tft_update();
		}
	}
}

void USART1_IRQHandler(void){
	char data;
	ticks_last = get_ticks();
	if (USART_GetITStatus(USART1, USART_IT_RXNE)==RESET) return;
	led_control(LED_D1_Pin, 1);

	tft_clear();
	data = USART_ReceiveData(USART1);
	tft_prints(0, 0, "%c",data);
	
	switch(data){
		case 'w':
			m1 = 0;
			m2 = 100;
			m3 = -100;
			break;
		case 'a':
			m1 = -100;
			m2 = 100 * 0.57735026919;
			m3 = -100 * 0.57735026919;
			break;
		case 'd':
			m1 = 100;
			m2 = -100 * 0.57735026919;
			m3 = 100 * 0.57735026919;
			break;
		case 's':
			m1 = 0;
			m2 = -100;
			m3 = 100;
			break;
	}
	
	print_graph();
	tft_update();
	uart_tx_byte(COM1, data);
	
	led_control(LED_D1_Pin, 0);
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}
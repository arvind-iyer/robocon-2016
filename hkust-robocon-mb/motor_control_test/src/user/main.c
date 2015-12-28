#include "main.h"
#include "math.h"

u16 ticks_last = 0;
int16_t m[3];
u8 need_update, input = 0;
u8 dm[3];

void print_graph(){
	tft_prints(6, 1, "_____>");
	tft_prints(8, 2, "|");
	tft_prints(8, 3, "|");
	tft_prints(7, 0, "%d", m[0]);
	
	tft_prints(7, 4, "/");
	tft_prints(6, 5, "/");
	tft_prints(5, 6, "\\");
	tft_prints(3, 5, "^\\");
	tft_prints(4, 7, "%d", m[1]);
	
	tft_prints(9, 4, "\\");
	tft_prints(10, 5, "\\");
	tft_prints(10, 6, "</");
	tft_prints(12, 5, "/");
	tft_prints(12, 7, "%d", m[2]);
}

void updateSpeed(u16 degree){
	dm[0] = 100 * cos(degree - 90);
	dm[1] = 100 * cos(degree + 60);
	dm[2] = 100 * cos(degree - 120);
}

int main(void)
{
	tft_init(1, RED, BLACK, BLUE);
	can_init();
	can_rx_init();
	uart_init(COM1, 115200);
	uart_interrupt(COM1);
	led_init();
	
	while(1){
		
		u8 i = 0;
		while(i<3){
			if (((dm[i]-m[i])*(dm[i]-m[i]))<20){
				m[i] = dm[i];
			}else {
				m[i] = dm[i]>m[i] ? m[i]+2 : m[i]-2;
			}
			i++;
		}
		
		if (get_ticks()%100 == 0){
			if (need_update==1){
				need_update = 0;
				updateSpeed(input);
				print_graph();
				tft_update();
			}else{
				m[0] = m[1] = m[2] = 0;
				tft_clear();
				print_graph();
				tft_update();
		}
		}
	}
}


void USART1_IRQHandler(void){
	char data;
	if (USART_GetITStatus(USART1, USART_IT_RXNE)==RESET) return;
	input = data;
	need_update = 1;
	led_control(LED_D1_Pin, 1);

	tft_clear();
	data = USART_ReceiveData(USART1);
	tft_prints(0, 0, "%c",data);
	uart_tx_byte(COM1, data);
	
	led_control(LED_D1_Pin, 0);
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}
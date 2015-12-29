#include "main.h"
#include "math.h"

u16 ticks_last = 0;
u8 state = 0;
u8 string_pointer = 0;
u8 input_string[128];

int main(void)
{
	led_init();
	tft_init(0, RED, WHITE, BLACK);
	ticks_init();
	
	uart_init(COM1, 115200);
	uart_interrupt(COM1);
	
	can_chat_init(0x0B0);
	
	while(1){
		u32 clock_ticks = get_ticks();
		if (clock_ticks%150==0){
			led_control(LED_D3_Pin, clock_ticks%300);
		}
	}
}


void USART1_IRQHandler(void){
	char data;
	if (USART_GetITStatus(USART1, USART_IT_RXNE)==RESET) return;
	led_control(LED_D1_Pin, 1);
	data = USART_ReceiveData(USART1);
	
	if (data==13){
		//Enter
		SendChatMessage(0x0B1, input_string, string_pointer+1);
		string_pointer = 0;
		tft_clear();
		tft_update();
	}else if(data==8){
		//Backspace
		string_pointer--;
		tft_prints(string_pointer%15, string_pointer/15 , " ");
		tft_update();
	}else{
		
		tft_prints(string_pointer%15, string_pointer/15 , "%c", data);
		tft_update();
		
		input_string[string_pointer++] = data;
		
		uart_tx_byte(COM1, data);	
	}
	
	led_control(LED_D1_Pin, 0);
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}
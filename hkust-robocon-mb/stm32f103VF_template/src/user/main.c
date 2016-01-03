#include "main.h"
#include "math.h"
#define PI 3.14159265
#define MAXVEL 100

u16 ticks_last = 0;
u8 state = 0;
u8 string_pointer = 0;
u8 input_string[128];

void can_motor_set_angle(int angle)
{
	motor_set_vel(MOTOR1, sin((angle)*PI/180)*MAXVEL*(-1), CLOSE_LOOP);
	motor_set_vel(MOTOR2, sin((angle+120)*PI/180)*MAXVEL*(-1), CLOSE_LOOP);
	motor_set_vel(MOTOR3, sin((angle+240)*PI/180)*MAXVEL*(-1), CLOSE_LOOP);
}

int main(void)
{
	led_init();
	tft_init(0, BLUE, WHITE, BLACK);
	ticks_init();
	
	uart_init(COM1, 115200);
	uart_interrupt(COM1);
	
	can_init();
	can_rx_init();
	can_motor_init();
	
	while (1) {
		//motor_set_vel(MOTOR1, 
	}
	
	/*
	can_chat_init(0x0B0);
	
	while(1){
		u32 clock_ticks = get_ticks();
		if (clock_ticks%150==0){
			led_control(LED_D3_Pin, clock_ticks%300);
		}
	}
	*/
}

/*
void USART1_IRQHandler(void){
	char data;
	if (USART_GetITStatus(USART1, USART_IT_RXNE)==RESET) return;
	led_control(LED_D1_Pin, 1);
	data = USART_ReceiveData(USART1);
	
	if (data==13){
		//Enter
		SendChatMessage(0x0B0, input_string, string_pointer);
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
	}
	
	led_control(LED_D1_Pin, 0);
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}
*/
#include "main.h"
#include <string.h>

TM_SERVO_t Servo1, Servo2;

void handleCommand(char * command) {
	int dataIndex = 0, contentIndex = 0, header = -1;

	int contents[16];

	for (char * data = strtok(command, "|"); data != NULL; data = strtok(NULL, "|")) {
		if (dataIndex == 0) {
			header = atoi(data);
		} else {
			contents[contentIndex++] = atoi(data);
		}
		dataIndex++;
	}

	switch (header) {
	case 0: // bla
		TM_SERVO_SetDegrees(&Servo1, 30);
		LED_ON(LED_1);
		LED_ON(LED_2);
		LED_ON(LED_3);
		LED_ON(LED_4);
		LED_ON(LED_5);
		break;
	case 1:
		TM_SERVO_SetDegrees(&Servo1, 90);
		LED_OFF(LED_1);
		LED_OFF(LED_2);
		LED_OFF(LED_3);
		LED_OFF(LED_4);
		LED_OFF(LED_5);
		break;
	}
}

int main(void) {
	SystemInit();
	led_init();

	//IMPORTANT: TM_DELAY_INIT MUST BE BEFORE ticks_init!!!
	TM_DELAY_Init();
	TM_DELAY_EnableDelayTimer();
	ticks_init();

	//Initialize the 2 Servos' Pins (plz see TM_stm32f4_pwm.c for the channel and pinspack)
	TM_SERVO_Init(&Servo1, TIM4, TM_PWM_Channel_3, TM_PWM_PinsPack_1);
	TM_SERVO_Init(&Servo2, TIM4, TM_PWM_Channel_4, TM_PWM_PinsPack_1);

	TM_USART_Init(USART1, TM_USART_PinsPack_1, 115200);

	uint16_t ticks_ms_img = 0;
	
	char buffer[512];
	
	while (1) {
		TM_USART_Puts(USART1, "Benchod\n");
		if (TM_USART_Gets(USART1, buffer, 512) > 0) {
			handleCommand(buffer);
		}
	}

	return 0;
}
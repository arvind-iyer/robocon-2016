#include "main.h"
#include <string.h>

TM_SERVO_t Servo1, Servo2;

//Command Handler for Raspberry Pi Controller
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

	//Initialize the 2 USART Ports
    char buffer[512];
    TM_USART_Init(USART1, TM_USART_PinsPack_1, 115200);
    TM_USART_Init(USART3, TM_USART_PinsPack_1, 115200);

	//Initialize I2C Modules (For future Gyro using I2C protocol, not CAN anymore) PB10 & PB12
    //TM_I2C_Init(I2C2, TM_I2C_PinsPack_1, 100000); 
	
    
    tft_init(PIN_ON_RIGHT,BLACK,WHITE,RED); //Init LCD
    tft_put_logo(110, 90); //Put a fancy robotics logo shit :)
    
    //Initialize the CAN protocol
    can_init();
    can_rx_init();
    can_motor_init();
    
    //Initialize encoder
    encoder_init();
    
	while (1) {
		if (TM_USART_Gets(USART1, buffer, 512) > 0) {
			handleCommand(buffer);
		}
        tft_prints(0,0,"HAHAHAH");
        tft_prints(0,1,"SO HAPPY :D!");
        TM_USART_Puts(USART1, "Benchod\n");
        tft_update();
        if(get_ms_ticks() % 200 == 0){
            LED_ON(LED_1);
            LED_ON(LED_2);
        }
        if(get_ms_ticks() % 200 == 100){
            LED_OFF(LED_1);
            LED_OFF(LED_2);
        }
        tft_prints(0,3,"Motor 1: %d",get_encoder_value(MOTOR1));
        tft_prints(0,4,"Motor 2: %d",get_encoder_value(MOTOR2));
        tft_prints(0,5,"Motor 3: %d",get_encoder_value(MOTOR3));
        
	}

	return 0;
}
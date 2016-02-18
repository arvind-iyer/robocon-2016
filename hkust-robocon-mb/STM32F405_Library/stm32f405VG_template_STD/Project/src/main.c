#include "main.h"
#include <string.h>


//Create 2 servo objects
TM_SERVO_t Servo1, Servo2;

//Result array of the sensor bar is stored here
extern u16 sensorbar_result[16];

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
    
    char buffer[512];

	led_init();

	//IMPORTANT: TM_DELAY_INIT MUST BE BEFORE ticks_init!!!
	TM_DELAY_Init();
	

	//Initialize the 2 Servos' Pins (plz see TM_stm32f4_pwm.c for the channel and pinspack)
	TM_SERVO_Init(&Servo1, TIM4, TM_PWM_Channel_3, TM_PWM_PinsPack_1);
	TM_SERVO_Init(&Servo2, TIM4, TM_PWM_Channel_4, TM_PWM_PinsPack_1);
    
    //TM_USART_Init(USART3, TM_USART_PinsPack_1
    //LCD Initialization
    tft_init(PIN_ON_BOTTOM,BLACK,WHITE,RED); //Init LCD
    
    //Initialize Gyro module
    gyro_init();

	//Initialize I2C Modules (For future Gyro using I2C protocol, not USART anymore) PB10 & PB12
    //TM_I2C_Init(I2C2, TM_I2C_PinsPack_1, 100000); 
    
    //Initialize the CAN protocol
    can_init();
    can_rx_init();
    can_motor_init();

    
    //Initialize encoder
    u32 ticks_ms_img = 0;
	while (1) {
        if(get_ticks() != ticks_ms_img){
            ticks_ms_img = get_ticks();
            tft_clear();
            tft_update();
        }    
	}

	return 0;
}
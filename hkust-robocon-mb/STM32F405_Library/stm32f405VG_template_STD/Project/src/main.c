#include "main.h"
#include <string.h>

TM_SERVO_t Servo1, Servo2;


//Sensor bar data
u16 data1[8];
u16 data2[8];
u16 sensor_output[16];

void receive(CanRxMsg msg)
{
	for(int i = 0; i < 8 ;i++){
        data1[i] = msg.Data[i];
    }
    
}

void receive2(CanRxMsg msg){
    for(int i = 0; i < 8 ; i++){
        data2[i] = msg.Data[i]; 
    }
}

void fill_array(){
    for(int i = 0; i < 8; i++){
        sensor_output[i] = data2[7-i];
    }
    for(int i = 0; i < 8; i++){
        sensor_output[8+i] = data1[7-i];
    }
}

void print_array(){
    tft_prints(0,0,"Sensor output");
    for(int i = 0; i < 16 ;i++){
        
        tft_prints(i,1,"%d",sensor_output[i]);
    }


}

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
    //TM_USART_Init(USART3, TM_USART_PinsPack_1, 115200);
    
    //LCD Initialization
    tft_init(PIN_ON_RIGHT,BLACK,WHITE,RED); //Init LCD
    //tft_put_logo(110, 90);
    
    //Initialize Gyro module
    gyro_init();

	//Initialize I2C Modules (For future Gyro using I2C protocol, not CAN anymore) PB10 & PB12
    //TM_I2C_Init(I2C2, TM_I2C_PinsPack_1, 100000); 
	
    

    
    //Initialize the CAN protocol
    can_init();
    can_rx_init();
    can_rx_add_filter(0x0C5,CAN_RX_MASK_DIGIT_0_F,receive);
    can_rx_add_filter(0x0C6,CAN_RX_MASK_EXACT,receive2);
    //can_motor_init();
    
    //Initialize encoder
    u32 ticks_ms_img = 0;
	while (1) {
        if(get_ms_ticks() != ticks_ms_img){
            ticks_ms_img = get_ms_ticks();
            tft_prints(0,0,"X: %d",get_X());
            tft_prints(0,1,"Y: %d",get_Y());
            tft_prints(0,2,"Angle: %d",get_angle());
            tft_update();

            
            //TM_USART_Puts(USART1, "Benchod\n");
            //TM_USART_Puts(USART3, "BENCHOD\n");
            if(get_ms_ticks() % 200 == 0){
                LED_ON(LED_1);
                LED_ON(LED_2);
 
            }
            if(get_ms_ticks() % 200 == 100){
                LED_OFF(LED_1);
                LED_OFF(LED_2);
            }
            
        }    
	}

	return 0;
}
#include "main.h"
#include <string.h>

//Result array of the sensor bar is stored here
extern u16 sensorbar_result[16];

//Create 2 Servo Objects
TM_SERVO_t Servo1,Servo2;

void bluetooth_listener(const uint8_t byte);

int main(void) {
    char buffer[512];
    
    
    //Create servo object

    //Initiate LED
	led_init();

	//Ticks initialization
	TM_DELAY_Init();
	

	//Initialize the 2 Servos' Pins (plz see TM_stm32f4_pwm.c for the channel and pinspack)
	TM_SERVO_Init(&Servo1, TIM4, TM_PWM_Channel_3, TM_PWM_PinsPack_1);
	//TM_SERVO_Init(&Servo2, TIM4, TM_PWM_Channel_4, TM_PWM_PinsPack_1);
    
    uart_init(COM1,115200);
    uart_interrupt_init(COM1,&bluetooth_listener);
    //uart_interrupt_init(COM3,&bluetooth_listener);
    
    //LCD Initialization
    tft_init(PIN_ON_BOTTOM,BLACK,WHITE,RED); //Init LCD
    
    //Initialize Gyro module(UART3)
    gyro_init();
    
    //Initialize the CAN protocol
    can_init();
    can_rx_init();
    
    //Initialize encoder  
    u32 ticks_ms_img = 0;
    
    
    
	while (1) {
        if(get_ticks() != ticks_ms_img){
            ticks_ms_img = get_ticks();

            tft_prints(0,0,"Count: %d",get_ticks());
            if(get_ticks() % 100 == 50){
                
                tft_update();
            }
        }    
	}

	return 0;
}

void bluetooth_listener(const uint8_t byte){
    switch(byte){
       //Do something
    }
}
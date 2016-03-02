#include "main.h"
#include <string.h>

extern u16 sensorbar_result[16];



//Create a Servo Object
TM_SERVO_t Servo1;

void bluetooth_listener(const uint8_t byte);

int main(void) {
    //Initialize System
    SystemInit();
    SystemCoreClockUpdate();
    
    //Initiate LED
	led_init();

	//Ticks initialization
	TM_DELAY_Init();
	

	//Initialize the 2 Servos' Pins (plz see TM_stm32f4_pwm.c for the channel and pinspack)
	TM_SERVO_Init(&Servo1, TIM3, TM_PWM_Channel_2, TM_PWM_PinsPack_1);
 
    //Bluetooth initialization
    uart_init(COM1,115200);
    uart_interrupt_init(COM1,&bluetooth_listener);
    
    //LCD Initialization
    tft_init(PIN_ON_BOTTOM,BLACK,WHITE,RED); //Init LCD
    
    //Initialize Gyro module(UART3)
    gyro_init();
    
    //Initialize the CAN protocol for motor
    //    can_init();
    //    can_rx_init();
    //    can_motor_init();
    
    //Initialize timer variable  
    u32 ticks_ms_img = 0;
    
    //Initialize Infrared Sensors
    infrared_sensor_init();
    
    //Initialize CAN based sensor bars
    sensorbar_init();
    
    //Initialize buzzer
    buzzer_init();
    
	while (1) {
        if(get_ticks() != ticks_ms_img){
            ticks_ms_img = get_ticks();
            tft_clear();
            tft_prints(0,3,"Count: %d",get_ticks());
            tft_prints(0,4,"Output: %d",read_infrared_sensor(INFRARED_1_PIN));
            tft_update();
        }
	}

	return 0;
}

void bluetooth_listener(const uint8_t byte){
    switch(byte){
       //Do something
    }
}
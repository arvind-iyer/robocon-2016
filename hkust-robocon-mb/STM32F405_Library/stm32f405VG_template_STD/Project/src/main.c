#include "main.h"
#include <string.h>

extern u16 sensorbar_result[16];



//Create 2 Servo Objects
TM_SERVO_t Servo1,Servo2;

void bluetooth_listener(const uint8_t byte);

int main(void) {
    //Initiate LED
	led_init();

	//Ticks initialization
	TM_DELAY_Init();
	

	//Initialize the 2 Servos' Pins (plz see TM_stm32f4_pwm.c for the channel and pinspack)
	TM_SERVO_Init(&Servo1, TIM4, TM_PWM_Channel_3, TM_PWM_PinsPack_1);
	TM_SERVO_Init(&Servo2, TIM4, TM_PWM_Channel_4, TM_PWM_PinsPack_1);
    
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
    
    //Initialize ELEC1100 line sensors
    line_sensor_init();
    
    //Initialize CAN based sensor bars
    sensorbar_init();
    
	while (1) {
        if(get_ticks() != ticks_ms_img){
            ticks_ms_img = get_ticks();
            fill_sensorbar_array();
            print_sensorbar_array();
            tft_prints(0,3,"Count: %d",get_ticks());
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
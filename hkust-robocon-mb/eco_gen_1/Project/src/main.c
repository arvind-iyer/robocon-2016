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
    
    //Initialize timer variable  
    u32 ticks_ms_img = 0;
    
    //Initialize ELEC1100 line sensors
    line_sensor_init();
    
    //Initialize CAN based sensor bars
    sensorbar_init();
    
    //For Eco Robot's filtering shit
    int begin = -1;
    int  end = 0;
    int length = 0;
    int lastMovement = 0;
    int lastTurn = 0;
    int lineSensorState;
    float factor = 0;
    
	while (1) {
        if(get_ticks() != ticks_ms_img){
            ticks_ms_img = get_ticks();
            fill_sensorbar_array();
            print_sensorbar_array();
            tft_prints(0,3,"Count: %d",get_ticks());
            lineSensorState = get_line_sensor_eco_robot();
            }
        for (int i = 0; i < 16; i++) {
				 uint32_t el = sensorbar_result[i];
				 if (el == 1) {
					 if (begin == -1) {
						 begin = i;
					 } else {
						 end = i;
						 length++;
					 }
				 }
			 }
			 
			 // if last turn was less than 1500ms ago, go in last direction.
			 if (get_ticks() - lastTurn >= 300) {
				 switch(lineSensorState){
					 case 1:
						 if(length>=7){
							lastMovement = SERVO_MICROS_MAX;
							lastTurn = get_ticks();
						 }
					 break;
					 case 2:
						 if(length>=7){
							lastMovement = SERVO_MICROS_MIN;
							lastTurn = get_ticks();
						 }
					 break;
					 default:
						 if (length >= 3 && length <= 6) {
							 factor = (((begin+1) + (end+1)) / 2) / (float) 16;
							 lastMovement = (SERVO_MICROS_MAX-200) - (factor * (SERVO_MICROS_MAX - SERVO_MICROS_MIN-400));
							 tft_prints(0, 4, "Fek: %.4f", factor); 
						 } 
							else {
								//fuck it, do last direction
							}
						}	
				}
			 if(ticks_ms_img%50 == 0){
				tft_update();
			 }
			 
			 // do processing shiet
			 TM_SERVO_SetMicros(&Servo1,lastMovement);
			 
			 begin = -1;
			 length = 0;
	}
	return 0;
}

void bluetooth_listener(const uint8_t byte){
    switch(byte){
       //Do something
    }
}
#include "main.h"
#include <string.h>
#include <stdbool.h>
u8 data1[8];
u8 data2[8];
u8 sensorbar_result[16];

void receive(CanRxMsg msg){
    for(int i = 0; i < 8 ;i++){
        data1[i] = msg.Data[i];
    }
}
void receive2(CanRxMsg msg){
    for(int i = 0; i < 8 ; i++){
        data2[i] = msg.Data[i]; 
    }
}
void fill_sensorbar_array(){
    for(int i = 0; i < 8; i++){
        sensorbar_result[i] = data2[7-i];
    }
    for(int i = 0; i < 8; i++){
        sensorbar_result[8+i] = data1[7-i];
    }
}

void print_array(){
    tft_prints(0,0,"Sensor output");
    for(int i = 0; i < 16 ;i++){
        tft_prints(i,1,"%d",sensorbar_result[i]);
    }
}


void bluetooth_listener(const uint8_t byte);

int main(void) {
    //Initialize system clock speed
    SystemInit();
    SystemCoreClockUpdate();
    
    //Initiate LED
	led_init();

	//Ticks initialization
	TM_DELAY_Init();
    
	//Initialize the 2 Servos' Pins (plz see TM_stm32f4_pwm.c for the channel and pinspack)
    TM_SERVO_t Servo1, Servo2;
    TM_SERVO_Init(&Servo1, TIM3, TM_PWM_Channel_2, TM_PWM_PinsPack_1);
    TM_SERVO_Init(&Servo2, TIM3, TM_PWM_Channel_1, TM_PWM_PinsPack_1);
    
    //LCD Initialization
    tft_init(PIN_ON_BOTTOM,BLACK,WHITE,RED); //Init LCD
    
    //Initialize timer variable  
    u32 ticks_ms_img = 0;
    
    //Initialize CAN based sensor bars
    can_init();
    can_rx_init();
    can_rx_add_filter(0x0C5, CAN_RX_MASK_EXACT,receive);
    can_rx_add_filter(0x0C6,CAN_RX_MASK_EXACT,receive2);
    
    //Init Infrared
    infrared_sensor_init();
    
    int length_state = 0;
    int begin = -1;
    int end = 0;
    int length = 0;
    int lastMovement = SERVO_MICROS_MID;
    int lastTurn = 0;
    float factor = 0;
    buzzer_init();
 
    
	while (1) {

      //if(get_ticks() % 100 == 0)tft_clear();
      //Initial processing and shit
      tft_clear();
      fill_sensorbar_array();
      
      
      tft_prints(0,2,"Count: %d",get_ticks());
      tft_prints(0,5,"IR left: %d",read_infrared_sensor(INFRARED_SENSOR_1));
      tft_prints(0,6,"IR right: %d",read_infrared_sensor(INFRARED_SENSOR_2));
      for (int k = 0; k < 16; k++) { 
        if(sensorbar_result[k] == 9)sensorbar_result[k] = 0;
            int el = sensorbar_result[k];
            if (el == 0) {
            if (begin == -1) begin = k; 
        else {
            end = k;
            length++;
        }
            }   
        }
        tft_prints(0,4,"length: %d",length);
        print_array();
        if (get_ticks() - lastTurn >= 250) {
            if(read_infrared_sensor(INFRARED_SENSOR_1) == 1){
                lastMovement = SERVO_MICROS_RIGHT;
            }
            else if(read_infrared_sensor(INFRARED_SENSOR_2) == 1){
                lastMovement = SERVO_MICROS_LEFT;
            }
            else{
            if(begin < 3 && length != 0 ){
                lastMovement = SERVO_MICROS_LEFT;
            }
            else if(end > 13 && length != 0 ){
                lastMovement = SERVO_MICROS_RIGHT;
            }
            
            else if (length >= 3 && length <= 6) {
                float factor = ((begin + end) / 2) / (float) 16;
                tft_prints(0,3,"factor=%f",factor);
                lastMovement = (SERVO_MICROS_LEFT) - (factor * (SERVO_MICROS_LEFT - SERVO_MICROS_RIGHT));
            } 
            
        //((begin + end)/2) < 6

        else if (length > 5){ // 90 degree turnnnzzz
            if ((begin + end) / 2 < 8) {
                lastMovement = SERVO_MICROS_LEFT;
            } 
            else {
                lastMovement = SERVO_MICROS_RIGHT;
            }
            lastTurn = get_ticks();
        }
    }
        TM_SERVO_SetMicros(&Servo1,lastMovement);
        begin = -1;
    }
        length = 0;
        tft_update();
        
	}

	return 0;
}

void bluetooth_listener(const uint8_t byte){
    switch(byte){
       //Do something
    }
}
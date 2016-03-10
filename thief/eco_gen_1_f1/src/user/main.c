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
    //Initiate LED
	led_init();

	//Ticks initialization
	ticks_init();
    
	//Initialize the 2 Servos' Pins (plz see TM_stm32f4_pwm.c for the channel and pinspack)
    servo_init();
    
    //LCD Initialization
    tft_init(3,BLACK,WHITE,RED); //Init LCD
    //Initialize timer variable  
    u32 ticks_ms_img = 0;
    
    //Initialize CAN based sensor bars
    can_init();
    can_rx_init();
    can_rx_add_filter(0x0C5, CAN_RX_MASK_EXACT,receive);
    can_rx_add_filter(0x0C6,CAN_RX_MASK_EXACT,receive2);
    
    //for sensorbar filtering shit
    int length_state = 0;
    int begin = -1;
    int end = 0;
    int length = 0;
    int lastMovement = SERVO_MICROS_MID;
    int lastTurn = 0;
    float factor = 0;
    buzzer_init();
	while (1) {
        if(get_full_ticks() != ticks_ms_img){
              tft_clear();
              //Initial processing and shit
              ticks_ms_img = get_full_ticks();
              fill_sensorbar_array();

              tft_prints(0,2,"Count: %d",get_full_ticks());
              for (int k = 0; k < 16; k++) {
                if(sensorbar_result[k] == 9)sensorbar_result[k] = 0;
                    int el = sensorbar_result[k];
                    if (el == 1) {
                    if (begin == -1) begin = k; 
                else {
                    end = k;
                    length++;
                }
                    }  
                }
                //2 3.5
                print_array();
                tft_prints(0,4,"length: %d",length);
                if (get_full_ticks() - lastTurn >= 800) {
                    if (length >= 3 && length <= 6) {
                        float factor = ((begin + end) / 2) / (float) 16;
                        tft_prints(0,3,"factor=%f",factor);
                        lastMovement = (SERVO_MICROS_LEFT) - (factor * (SERVO_MICROS_LEFT - SERVO_MICROS_RIGHT));
                    } 
                else if(length == 16 || length == 15){
                    lastMovement = SERVO_MICROS_MID;
                }
                else if (length > 6){ // 90 degree turnnnzzz
                    if ((begin + end) / 2 < 8) {
                        lastMovement = SERVO_MICROS_LEFT;
                    } 
                    else {
                        lastMovement = SERVO_MICROS_RIGHT;
                    }
                    lastTurn = get_full_ticks();
                }
                servo_control_all(lastMovement);
                begin = -1;
            }
                length = 0;
                tft_prints(0,5,"servo= %d",lastMovement);
                tft_prints(0,6,"lastTurn=%d",lastTurn);
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
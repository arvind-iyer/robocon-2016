#include "main.h"

u8 data1[8];
u8 data2[8];
u8 sensorbar_result[16];
u8 river;
u8 hueAvg;
u8 border;
u8 globalState = NOT_RIVER;
u8 prevStage = NOT_RIVER;
bool inBlue = false;
bool sensorIsFlipped = true;

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

void receive3(CanRxMsg msg){
    river = msg.Data[0];
    hueAvg = msg.Data[1];
    border = msg.Data[2];
}

void fill_sensorbar_array(){
    for(int i = 0; i < 8; i++){
        if(sensorIsFlipped) sensorbar_result[i] = data2[7-i];
        else sensorbar_result[i] = data1[i];
    }
    for(int i = 0; i < 8; i++){
        if(sensorIsFlipped) sensorbar_result[8+i] = data1[7-i];
        else sensorbar_result[8+i] = data2[i];
    }
}

void print_array(){
    tft_prints(0,0,"Sensor output");
    for(int i = 0; i < 16 ;i++){
        
        tft_prints(i,1,"%d",sensorbar_result[i]);
    }
}

int main(void) {
	SystemCoreClockUpdate();
	led_init();			//Initiate LED
	ticks_init();		//Ticks initialization
    tft_init(PIN_ON_BOTTOM,BLACK,WHITE,RED);     //LCD Initialization
	buzzer_init();	//Initialize buzzer
    servo_init();
    
    buzzer_init();
    
	//Initialize the CAN protocol for motor
    can_init();
    can_rx_init();
    can_rx_add_filter(0x0C5,CAN_RX_MASK_EXACT,receive);
    can_rx_add_filter(0x0C6,CAN_RX_MASK_EXACT,receive2);
    can_rx_add_filter(0x0C7,CAN_RX_MASK_EXACT,receive3);
	
    bool fullWhite = false;
    //Init Infrared sensor
    infrared_sensor_init();
   
	u32 ticks_ms_img = 0;
    
    int length_state = 0;
    int begin = -1;
    int end = 0;
    int length = 0;
    int lastMovement = SERVO_MICROS_MID;
    int lastTurn = 0;
    float factor = 0;
    encoder_init();
	while (1) {
            tft_clear();
            tft_prints(0,2,"count :%d",get_full_ticks());
            //Initial processing and shit
            fill_sensorbar_array();
            tft_prints(0,3,"i1:%d i2:%d",read_infrared_sensor(INFRARED_SENSOR_1),read_infrared_sensor(INFRARED_SENSOR_2));
            for(int k = 0; k < 16; k++) {
                int el = sensorbar_result[k];
                if (el == 1) {
                    if (begin == -1) begin = k; 
                    else {
                        end = k;
                    }
                    length++;
                }   
            }
            tft_prints(0,4,"length: %d",length);
            print_array();
            
            switch(globalState){
                case NOT_RIVER:
                    if (get_full_ticks() - lastTurn >= 500){
                        if(length == 16 && fullWhite == false){
                            lastMovement = SERVO_MICROS_RIGHT;
                            fullWhite = true;
                            lastTurn = get_full_ticks();
                        }
                        
                        else if (length == 16 && fullWhite == true){
                            lastMovement = SERVO_MICROS_MID;
                        }

                        else if (length >= 2 && length <= 7) {
                            float factor = ((begin + end) / 2) / (float) 16;
                            lastMovement = (SERVO_MICROS_LEFT) - (factor * (SERVO_MICROS_LEFT - SERVO_MICROS_RIGHT));
                        } 

                        //((begin + end)/2) < 6
                        else if (length > 7 && length < 16){
                            if ((begin + end) / 2 < 8 ){
                                lastMovement = SERVO_MICROS_LEFT;
                            }
                            else if((begin + end) / 2 >= 8 && river && inBlue){
                                lastMovement = SERVO_MICROS_RIGHT - 200;
                                globalState = STAGE1;
                            }
                            else lastMovement = SERVO_MICROS_RIGHT;
                        }  
                    begin = -1;
                    servo_control(SERVO1,lastMovement);
                }
                if(river && !read_infrared_sensor(INFRARED_SENSOR_2))inBlue = true;
                if(!river)inBlue = false;
                break;
                case STAGE1:
                    if(!river)globalState = NOT_RIVER;
                    if(!read_infrared_sensor(INFRARED_SENSOR_1)){
                        servo_control(SERVO1,SERVO_MICROS_MID - 150);
                        globalState = STAGE2;
                        reset_all_encoder();
                    }
                
                break;
                case STAGE2:
                    if(!river)globalState = NOT_RIVER;
                    if(get_count(ENCODER1) > 40000){
                        globalState = NOT_RIVER;
                        inBlue = false;
                    }
                break;
            }
    tft_prints(0,5,"fullwhite:%d",fullWhite);
    tft_prints(0,6,"e1: %d",get_count(ENCODER1));
    tft_prints(0,7,"state: %d",globalState);
    tft_prints(0,8,"riv:%d inB:%d",river,inBlue);
    tft_prints(0,9,"h: %d b: %d",hueAvg,border);
    length = 0;
    tft_update();
    }
    return 0;
}
	
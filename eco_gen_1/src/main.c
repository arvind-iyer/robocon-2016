#include "main.h"
u8 data1[8];
u8 data2[8];
u8 sensorbar_result[16];
u8 river;
u8 hueAvg;
u8 border;

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

int main(void) {
	SystemCoreClockUpdate();
	led_init();			//Initiate LED
	ticks_init();		//Ticks initialization
    tft_init(PIN_ON_BOTTOM,BLACK,WHITE,RED);     //LCD Initialization
	buzzer_init();	//Initialize buzzer
    servo_init();
    us_init(); //Initialize Ultra-sonic sensor
    
    encoder_init();
	

	//Initialize the CAN protocol for motor
    can_init();
    can_rx_init();
    can_rx_add_filter(0x0C5, CAN_RX_MASK_EXACT,receive);
    can_rx_add_filter(0x0C6,CAN_RX_MASK_EXACT,receive2);
    can_rx_add_filter(0x0C7,CAN_RX_MASK_EXACT,receive3);
	
    bool fullWhite = false;
    
    //Get Clock information
    RCC_ClocksTypeDef  rcc_clocks;
	RCC_GetClocksFreq(&rcc_clocks);
    
    //Init Infrared sensor
    infrared_sensor_init();
    
    //Initialize servo
	u32 ticks_ms_img = 0;
    
    int length_state = 0;
    int begin = -1;
    int end = 0;
    int length = 0;
    int lastMovement = SERVO_MICROS_MID;
    int lastTurn = 0;
    float factor = 0;
    int prev;
    //servo_control(SERVO4,SERVO_MICROS_RIGHT);
	while (1) {
//            //Initial processing and shit
            tft_clear();
            fill_sensorbar_array();
            tft_prints(0,2,"Count: %d",get_full_ticks());
            tft_prints(0,5,"IR left: %d",read_infrared_sensor(INFRARED_SENSOR_1));
            tft_prints(0,6,"IR right: %d",read_infrared_sensor(INFRARED_SENSOR_2));
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
            if (get_full_ticks() - lastTurn >= 500){
                
                //Emergency turning using infrared sensor
//                if(read_infrared_sensor(INFRARED_SENSOR_1) == 1 && read_infrared_sensor(INFRARED_SENSOR_2) == 0){
//                    lastMovement = SERVO_MICROS_RIGHT;
//                }
//                else if(read_infrared_sensor(INFRARED_SENSOR_2) == 1 && read_infrared_sensor(INFRARED_SENSOR_1) == 0){
//                    lastMovement = SERVO_MICROS_LEFT;
//                }
                
                //Using the sensorbar
                //else{
                    if(length == 16 && fullWhite == false){
                        lastMovement = SERVO_MICROS_RIGHT;
                        fullWhite = true;
                        lastTurn = get_full_ticks();
                    }
                    
                    else if (length == 16 && fullWhite == true){
                        lastMovement = SERVO_MICROS_MID;
                        //lastTurn = get_full_ticks();
                    }
//                    else if(begin == 1 && length != 0 ){
//                        lastMovement = SERVO_MICROS_LEFT + 100;
//                    }
//                    else if(begin == 15 && length != 0 ){
//                        lastMovement = SERVO_MICROS_RIGHT - 100;
//                    }

                    else if (length >= 2 && length <= 6) {
                        float factor = ((begin + end) / 2) / (float) 16;
                        lastMovement = (SERVO_MICROS_LEFT) - (factor * (SERVO_MICROS_LEFT - SERVO_MICROS_RIGHT));
                    } 

                    //((begin + end)/2) < 6
                    else if (length > 6 && length < 15){
                        if ((begin + end) / 2 < 8 ){
                            lastMovement = SERVO_MICROS_LEFT;
                        }
                        else{
                            lastMovement = SERVO_MICROS_RIGHT ;
                        }
                        if(river)lastTurn = get_full_ticks(); //Put delay only in river area
                    }  
            //}
            begin = -1;
            servo_control(SERVO3,lastMovement);
        }
    tft_prints(0,3,"river:%d",river);
    tft_prints(0,6,"fullwhite:%d",fullWhite);
    tft_prints(0,7,"Servo: %d ",lastMovement);
    tft_prints(0,8,"h:%d b:%d",hueAvg,border);
    length = 0;
    tft_update();
    }
    return 0;
}

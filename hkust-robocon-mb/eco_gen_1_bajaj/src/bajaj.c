#include "bajaj.h"

extern u8 data1[8];
extern u8 data2[8];
extern u8 sensorbar_result[16];
extern u8 river;
extern u8 hueAvg;
extern u8 border;
extern u8 globalState;

extern int begin;
extern int end;
extern int length;
extern int lastMovement;
extern int lastTurn;
extern float factor;
extern bool inBlue;
extern bool sensorIsFlipped;
extern bool fullWhite;
extern bool systemOn;

extern int yaw_of_imu;

float imuFactor;
int imuMovement;


void systemInit(){
    SystemCoreClockUpdate();
	led_init();			//Initiate LED
	ticks_init();		//Ticks initialization
    tft_init(PIN_ON_BOTTOM,BLACK,WHITE,RED);     //LCD Initialization
	buzzer_init();	//Initialize buzzer
    servo_init();
    buzzer_init();
    button_init();
    
    encoder_init();
    infrared_sensor_init();
      
	//Initialize the CAN protocol for motor
    can_init();
    can_rx_init();
    can_rx_add_filter(0x0C5,CAN_RX_MASK_EXACT,receive);
    can_rx_add_filter(0x0C6,CAN_RX_MASK_EXACT,receive2);
    can_rx_add_filter(0x0C7,CAN_RX_MASK_EXACT,receive3);
}

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
        if(sensorIsFlipped) sensorbar_result[i] = data2[7 - i];
        else sensorbar_result[i] = data1[i];
    }
    for(int i = 0; i < 8; i++){
        if(sensorIsFlipped) sensorbar_result[8+i] = data1[7 - i];
        else sensorbar_result[8 + i] = data2[i];
    }
}

void print_data(){
    tft_prints(0,0,"Sensor output");
    for(int i = 0; i < 16 ;i++) tft_prints(i,1,"%d",sensorbar_result[i]);
    tft_prints(0,2,"yaw :%d cal:%d",yaw_of_imu,ardu_imu_calibrated);
    tft_prints(0,3,"i1:%d i2:%d",read_infrared_sensor(INFRARED_SENSOR_1),read_infrared_sensor(INFRARED_SENSOR_2));
    tft_prints(0,4,"length: %d",length);
    tft_prints(0,5,"fullwhite:%d",fullWhite);
    tft_prints(0,6,"e1:%d e2:%d",get_count(ENCODER1),get_count(ENCODER2));
    tft_prints(0,7,"state: %d",globalState);
    tft_prints(0,8,"riv:%d inB:%d",river,inBlue);
    tft_prints(0,9,"system: %d",systemOn);    
}

void process_array(){
    for(int k = 0; k < 16; k++) {
        int el = sensorbar_result[k];
        if (el == 1) {
            length++;
            if (begin == -1) begin = k; 
            //Add more conditioning here
            else if(sensorbar_result[k+1] > 15){
                end = 15;
            }
            else if(sensorbar_result[k+1] == 0){
                end = k; //If the rest is zero, regards others as noise
                break;
            }
            else {
                end = k;
            }  
        }   
    }
}

void goNormal(void){
    if (get_full_ticks() - lastTurn >= 500){
        if(length >= 15 && fullWhite == false){
            lastMovement = SERVO_MICROS_RIGHT;
            fullWhite = true;
            lastTurn = get_full_ticks();
        }
        
        else if (length >= 15 && fullWhite == true){
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
                lastMovement = SERVO_MICROS_RIGHT + 250;
            }
            else lastMovement = SERVO_MICROS_RIGHT;
        }  
    begin = -1;
    servo_control(SERVO1,lastMovement);
    }
}

void goFindWall(void){
    if(!read_infrared_sensor(INFRARED_SENSOR_1)){
        servo_control(SERVO1,SERVO_MICROS_MID + 50);
        globalState = STAGE2;
        reset_all_encoder();
    }
}

void goStraightYolo(void){
    if(get_count(ENCODER1) > 45000){
        globalState = NOT_RIVER;
        inBlue = false;
    }
}

void goUsingImu(void){
    if((get_count(ENCODER1) > 30000) && !read_infrared_sensor(INFRARED_SENSOR_2)){
        globalState = STAGE2;
        reset_all_encoder();
    }
    else{
        if(yaw_of_imu >= -5 && yaw_of_imu <= 5)servo_control(SERVO1,SERVO_MICROS_MID);
        else if(yaw_of_imu > 120) servo_control(SERVO1,SERVO_MICROS_LEFT);
        else if(yaw_of_imu < -120) servo_control(SERVO1,SERVO_MICROS_RIGHT);
        else{
            //non-extreme angle movement
            imuFactor = (float)yaw_of_imu / 120;
            imuMovement = SERVO_MICROS_MID + (imuFactor * 600);
            servo_control(SERVO1,imuMovement);
        }
    }
}

void goDetectRightWall(void){
    if(river && !read_infrared_sensor(INFRARED_SENSOR_2)){
        servo_control(SERVO1,SERVO_MICROS_MID + 150);
        globalState = STAGE3;
    }
}

void goDetectLeftWall(void){
    if(river && !read_infrared_sensor(INFRARED_SENSOR_1)){
        globalState = NOT_RIVER;
    }
}

void goStraightLittleBit(void){
    servo_control(SERVO1,SERVO_MICROS_MID + 150);
    if(get_count(ENCODER1) > 16000){
        lastMovement = SERVO_MICROS_MID + 200;
        globalState = NOT_RIVER;
        lastMovement = SERVO_MICROS_MID + 200;
    }
}

void printSystemOff(void){
    tft_prints(0,3,"SYSTEM IS OFF");
    tft_prints(0,0,"Sensor output");
    for(int i = 0; i < 16 ;i++) tft_prints(i,1,"%d",sensorbar_result[i]);
    tft_prints(0,2,"River: %d",river);
    tft_prints(0,4,"calibrated:%d",ardu_imu_calibrated);
    tft_prints(0,5,"yaw:%f",ardu_cal_ypr[0]);
    tft_prints(0,6,"length:%d",length);
    tft_prints(0,7,"b:%d e:%d",begin,end);
    tft_prints(0,8,"i1:%d i2:%d",read_infrared_sensor(INFRARED_SENSOR_1),read_infrared_sensor(INFRARED_SENSOR_2));
}






#include "bajaj.h"

extern u8 data1[8];
extern u8 data2[8];
extern u8 sensorbar_result[16];
extern u8 river;
extern int hueAvg;
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

extern ZONE gameZone;
extern ZONE prevGameZone;

extern int sumOrange;
extern int sumDarkGreen;
extern ZONE prevGameZone;

float imuFactor;
int imuMovement = 0;
ZONE expectedGameZone = DARKGREENZONE;

bool colourIsDifferent = true;

void systemInit(){
    SystemCoreClockUpdate();
	ticks_init();		//Ticks initialization
    tft_init(PIN_ON_BOTTOM,BLACK,WHITE,RED);     //LCD Initialization
    encoder_init();
    infrared_sensor_init();
    buzzer_init();
    buzzer_play_song(MARIO_BEGIN,100,2);
	//Initialize the CAN protocol for sensorbar
    can_init();
    can_rx_init();
    can_rx_add_filter(0x0C5,CAN_RX_MASK_EXACT,receive);
    can_rx_add_filter(0x0C6,CAN_RX_MASK_EXACT,receive2);
    can_rx_add_filter(0x0C7,CAN_RX_MASK_EXACT,receive3);
    ardu_imu_init();
    servo_init();
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

void determineZone(){
    //Dark blue
    if(hueAvg >= 220 && hueAvg <= 240){gameZone = BLUEZONE;}
    
    //Orange
    else if(hueAvg <= 38 && hueAvg > 10){gameZone = ORANGEZONE;}
    
    //Light green
    else if(hueAvg >= 80 && hueAvg < 180){gameZone = LIGHTGREENZONE;}
    
    //Dark green
    else if(hueAvg >= 180 && hueAvg < 220){gameZone = DARKGREENZONE;}
    
    //Pink
    else if(hueAvg > 38 || hueAvg < 80){gameZone = PINKZONE;}
}

void sumColour(){
    if(gameZone == ORANGEZONE && colourIsDifferent && expectedGameZone == ORANGEZONE){
        reset_all_encoder();
        colourIsDifferent = false;
        if(get_count(ENCODER1) > 2000){
            sumOrange++;
            expectedGameZone = DARKGREENZONE;
            colourIsDifferent = true;
        }
    }
    else if(gameZone == DARKGREENZONE && colourIsDifferent && expectedGameZone == DARKGREENZONE){
        reset_all_encoder();
        colourIsDifferent = false;
        if(get_count(ENCODER1) > 2000){
            sumDarkGreen++;
            expectedGameZone = ORANGEZONE;
            colourIsDifferent = true;
        }
    }
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
    tft_prints(0,0,"sG:%d sO:%d",sumDarkGreen,sumOrange);
    for(int i = 0; i < 16 ;i++) tft_prints(i,1,"%d",sensorbar_result[i]);
    tft_prints(0,2,"yaw:%d cal:%d",yaw_of_imu,ardu_imu_calibrated);
    tft_prints(0,3,"left:%d right:%d",read_infrared_sensor(INFRARED_SENSOR_LEFT),read_infrared_sensor(INFRARED_SENSOR_RIGHT));
    tft_prints(0,4,"ul:%d ur:%d", read_infrared_sensor(INFRARED_SENSOR_UPPER_LEFT),read_infrared_sensor(INFRARED_SENSOR_UPPER_RIGHT));
    tft_prints(0,5,"le:%d hAvg:%d",length,hueAvg);
    tft_prints(0,6,"fullwhite:%d",fullWhite);
    tft_prints(0,7,"e1:%d e2:%d",get_count(ENCODER1),get_count(ENCODER2));
    tft_prints(0,8,"state: %d",globalState);
    tft_prints(0,9,"riv:%d gz:%d",river,gameZone);   
}

void process_array(){
    for(int k = 0; k < 16; k++) {
        int el = sensorbar_result[k];
        if (el == 1) {
            length++;
            if (begin == -1) begin = k; 
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
    if(get_full_ticks() - lastTurn >= 500){
        if(length == 16 && fullWhite == false && gameZone == ORANGEZONE){
            lastMovement = SERVO_MICROS_RIGHT - 300;
            fullWhite = true;
            lastTurn = get_full_ticks();
        }
        
        else if (length == 16 && fullWhite == true){
            lastMovement = SERVO_MICROS_MID;
        }

        else if (length >= 2 && length <= 6) {
            float factor = ((begin + end) / 2) / (float) 16;
            lastMovement = (SERVO_MICROS_LEFT) - (factor * (SERVO_MICROS_LEFT - SERVO_MICROS_RIGHT));
        } 
    }
    begin = -1;
    if(get_ticks() % 50 == 0)servo_control(SERVO1,lastMovement);
}

void goUsingImu(void){
    if((get_count(ENCODER1) > 16000) && !read_infrared_sensor(INFRARED_SENSOR_RIGHT)){
        globalState = STAGE2;
        reset_all_encoder();
    }
    yaw_of_imu = yaw_of_imu > 180 ? 180 : yaw_of_imu;
    yaw_of_imu = yaw_of_imu < -180 ? -180 : yaw_of_imu; 
    
    
    imuFactor = (float)yaw_of_imu / 180.0;
    imuMovement = SERVO_MICROS_MID + (imuFactor * 800);
    if(get_ticks() % 25 == 0)servo_control(SERVO1,imuMovement);
}

void goStraightLittleBit(void){
    servo_control(SERVO1,SERVO_MICROS_MID + 200);
    if(get_count(ENCODER1) > 4000){
        lastMovement = SERVO_MICROS_MID + 200;
        globalState = NOT_RIVER;
        lastMovement = SERVO_MICROS_MID + 200;
    }
}








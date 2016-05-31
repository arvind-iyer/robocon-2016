#include "bajaj.h"

//Values Define which will be filled after starting the program
int MAX_NINETY_TURNING;
int IMU_ANGLE1;
int IMU_ANGLE2;
int IMU_ANGLE3;
float NINETY_IMU;
int LESSER_TURNING;
int NINETY_TURNING;
int SLOPE_TURNING_RIGHT;
int SLOPE_TURNING_LEFT;
int SLOPE_ENCODER;
int DELAY;
GAMESIDE side;
INFRARED_SENSOR infrared1;
INFRARED_SENSOR infrared2;
extern bool systemOn;

int buttonWhiteCount = 0;
int buttonRedCount = 0;

extern u8 data1[8];
extern u8 data2[8];
extern u8 sensorbar_result[16];
extern u8 river;
extern u8 border;
extern u8 globalState;
extern int begin;
extern int end;
extern int length;
extern int lastMovement;
extern int lastTurn;
extern float factor;
extern bool sensorIsFlipped;
extern int fullWhite;
extern int yaw_of_imu;
extern int pitch_of_imu;
extern int hueAvg;
int passedRiver = 0;
int passedDownSlope = 0;
float imuFactor;
int imuMovement;
int sat1;
int value1;
uint32_t encoder_revolution = 0;
char gameZoneString[12]= "UNKNOWN";
ZONE gameZone;
ZONE expectedGameZone;
SLOPEZONE currentSlopeZone = STARTZONE;
char currentSlopeZoneString[10] = "STARTZONE";

void initializeValues(void){
    if(button_pressed(BUTTON_RED)){
        while(button_pressed(BUTTON_RED));
        tft_init(PIN_ON_BOTTOM,BLACK,ORANGE,RED); 
        IMU_ANGLE1 = -90;
        IMU_ANGLE2 = 30;
        IMU_ANGLE3 = -45;
        NINETY_IMU = -180;
        LESSER_TURNING = 260;
        SLOPE_TURNING_RIGHT = 1100;
        SLOPE_TURNING_LEFT = 1600;
        SLOPE_ENCODER = 46000;
        DELAY = 1000;
        side = REDSIDE;
        infrared1 = INFRARED_SENSOR_RIGHT;
        infrared2 = INFRARED_SENSOR_LEFT;
        buttonRedCount = 0;
        NINETY_TURNING = 950;
        systemOn = 1;
    }
    else if(button_pressed(BUTTON_WHITE)){
        while(button_pressed(BUTTON_WHITE));
        tft_init(PIN_ON_BOTTOM,BLACK,BLUE2,RED);
        IMU_ANGLE1 = 80;
        IMU_ANGLE2 = -30;
        IMU_ANGLE3 = 45;
        NINETY_IMU = 180;
        LESSER_TURNING = -180;
        SLOPE_TURNING_RIGHT = 1100;
        SLOPE_TURNING_LEFT = 1600;
        SLOPE_ENCODER = 45000;
        DELAY = 1000;
        side = BLUESIDE;
        infrared1 = INFRARED_SENSOR_LEFT;
        infrared2 = INFRARED_SENSOR_RIGHT;
        buttonWhiteCount = 0;
        NINETY_TURNING = 1700;
        systemOn = 1;
    }
}

void update_encoder(){
    if(encoder_revolution > 3)encoder_revolution = 0;
    if(get_count(ENCODER2) > (int)SLOPE_ENCODER){
        encoder_revolution++;
        reset_encoder_2();
    }
}

void systemInit(){
	led_init();			//Initiate LED
	ticks_init();		//Ticks initialization
    tft_init(PIN_ON_BOTTOM,BLACK,WHITE,RED);     //LCD Initialization
	buzzer_init();	//Initialize buzzer
    servo_init();
    button_init();
    encoder_init();
    infrared_sensor_init();
    //ardu_imu_init();
    MTi_1_UART_init();
    
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
    hueAvg = msg.Data[1];
}

void fill_sensorbar_array(){
    if(get_ticks() % 10 == 0){
        for(int i = 0; i < 8; i++){
            if(sensorIsFlipped) sensorbar_result[i] = data2[7 - i];
            else sensorbar_result[i] = data1[i];
        }
        for(int i = 0; i < 8; i++){
            if(sensorIsFlipped) sensorbar_result[8+i] = data1[7 - i];
            else sensorbar_result[8 + i] = data2[i];
        }
    }
}

void print_data(){
    tft_prints(0,0,"ZONE: %s",gameZoneString);
    for(int i = 0; i < 16 ;i++) tft_prints(i,1,"%d",sensorbar_result[i]);
    tft_prints(0,9,"y:%f",ardu_cal_ypr[0]);
    tft_prints(0,3,"il:%d ir:%d",read_infrared_sensor(INFRARED_SENSOR_LEFT),read_infrared_sensor(INFRARED_SENSOR_RIGHT));
    tft_prints(0,4,"ul:%d ur:%d", read_infrared_sensor(INFRARED_SENSOR_UPPER_LEFT),read_infrared_sensor(INFRARED_SENSOR_UPPER_RIGHT));
    tft_prints(0,5,"le:%d state:%d",length,globalState);
    tft_prints(0,6,"fw:%d, river:%d",fullWhite,river);
    tft_prints(0,7,"e1:%d e2:%d",get_full_count(ENCODER1),get_full_count(ENCODER2));
    tft_prints(0,8,"priv: %d pds:%d",passedRiver,passedDownSlope);
    tft_prints(0,2,"%s",currentSlopeZoneString); 
}

void process_array(){
    //Seperate the marking into a 2 stage process
    
    //scan left properties
    int begin_left = -1;
    int end_left;
    int length_left = 0;
    for(int k = 0; k < 16; k++) {
        int el = sensorbar_result[k];
        if (el == 1) {
            length_left++;
            if (begin_left == -1) begin_left = k;
            if(sensorbar_result[k + 1] == 0){
                end_left = k; //If the rest is zero, regards others as noise
                break;
            }
            else {
                end_left = k;
            }  
        }   
    }
    
    //TODO: scan from the right
    //scan from right properties
    int begin_right;
    int end_right = -1;
    int length_right = 0;
    for(int i = 15; i >= 0; i--) {
        int el = sensorbar_result[i];
        if (el == 1) {
            length_right++;
            if (end_right == -1) end_right = i;
            if(sensorbar_result[i - 1] == 0){
                begin_right = i; //If the rest is zero, regards others as noise
                break;
            }
            else {
                begin_right = i;
            }  
        }   
    }
    
    //Now compare left and right,regard other's shorter segment data as a noise
    if(length_left > length_right){
        length = length_left;
        begin = begin_left;
        end = end_left;
    }
    else{
        length = length_right;
        begin = begin_right;
        end = end_right;        
    }
    
}


void goNormal(void){
    servo_control(BAJAJ_SERVO,lastMovement);
    if (get_full_ticks() - lastTurn >= (int)DELAY){            
        if (length >= 1 && length <= 11) {
            if(fullWhite){
                float factor = ((begin + end) / 2) / (float) 16;
                lastMovement = (SERVO_MICROS_LEFT) - (factor * (SERVO_MICROS_LEFT - SERVO_MICROS_RIGHT));
            }
            else{
                float factor = ((begin + end) / 2) / (float) 16;
                lastMovement = (SLOPE_TURNING_LEFT) - (factor * (SLOPE_TURNING_LEFT - SLOPE_TURNING_RIGHT));
            }
        }
        if(gameZone == LIGHTGREENZONE && passedRiver){
            passedDownSlope = 1;
        }
    servo_control(BAJAJ_SERVO,lastMovement);
    }
}
void goNinety(void){
    switch(side){
        case REDSIDE:
            if((int)ardu_cal_ypr[0] > -75){
                fullWhite = 1;
                globalState = NOT_RIVER;
            }
        break;
        case BLUESIDE:
            if((int)ardu_cal_ypr[0] < 90){
                fullWhite = 1;
                globalState = NOT_RIVER;
            }
        break;
    }
    imuFactor = (float)ardu_cal_ypr[0] / 180.0f;
    lastMovement = NINETY_TURNING;
    servo_control(BAJAJ_SERVO,lastMovement);
}

void goUsingImu(void){
    imuFactor = ardu_cal_ypr[0] / 180.0f;
    lastMovement = imuMovement = SERVO_MICROS_MID + (imuFactor * 500);
    servo_control(BAJAJ_SERVO,lastMovement);
    
    //Stopping condition
    if((get_count(ENCODER1) > 20000) && !read_infrared_sensor(infrared2)){
        reset_encoder_1();
        globalState = STAGE2;
    }
}

void goUsingImu2(void){
    imuFactor = ardu_cal_ypr[0] / 180.0f;
    imuMovement = SERVO_MICROS_MID + (imuFactor * 500);
    servo_control(BAJAJ_SERVO,imuMovement);   
    
    //Stopping condition
    if((get_count(ENCODER1) > 3000) && !read_infrared_sensor(infrared1)){
        ardu_cal_ypr[0] = IMU_ANGLE3;
        reset_encoder_1();
        globalState = STAGE4;
    }
}

void goUsingImu3(void){
    imuFactor = ardu_cal_ypr[0] / 180.0f;
    imuMovement = SERVO_MICROS_MID + (imuFactor * 500);
    servo_control(BAJAJ_SERVO,imuMovement); 
    
    //Stopping condition
    if((get_count(ENCODER1) > 2000) && !read_infrared_sensor(infrared2)){
        reset_encoder_1();
        globalState = STAGE2; // Go to last stage of the river part
    }
}
void goStraightLittleBit(void){
    lastMovement = SERVO_MICROS_MID + (int)LESSER_TURNING;
    servo_control(BAJAJ_SERVO, lastMovement);
    //Stopping condition
    if(get_count(ENCODER1) > 7000){
        //lastMovement = SERVO_MICROS_MID + (int)LESSER_TURNING;
        //servo_control(BAJAJ_SERVO, lastMovement);
        passedRiver = 1;
        globalState = NOT_RIVER;
    }
}

void printSystemOff(void){
      tft_prints(0,0,"PRESS RED / WHITE");
      tft_prints(0,1,"e1:%d e2:%d",get_full_count(ENCODER1),get_full_count(ENCODER2));
      tft_prints(0,2,"zone: %s",gameZoneString);
      for(int i = 0; i < 16 ;i++) tft_prints(i,3,"%d",sensorbar_result[i]);
      tft_prints(0,4,"calibrated:%d",ardu_imu_calibrated);
      tft_prints(0,5,"yaw:%f",ardu_cal_ypr[0]);
      tft_prints(0,6,"length:%d fw:%d",length,fullWhite);
      tft_prints(0,7,"il:%d ir:%d",read_infrared_sensor(INFRARED_SENSOR_LEFT),read_infrared_sensor(INFRARED_SENSOR_RIGHT));
      tft_prints(0,8,"ul:%d ur:%d", read_infrared_sensor(INFRARED_SENSOR_UPPER_LEFT),read_infrared_sensor(INFRARED_SENSOR_UPPER_RIGHT));
        
    
//        tft_prints(0,0,"%d",get_MTi_acc(0));
//        tft_prints(0,1,"%d",get_MTi_acc(1));
//        tft_prints(0,2,"%d",get_MTi_acc(2));
//        tft_prints(0,3,"%d",get_MTi_ang(3));
//        tft_prints(0,4,"%d",get_MTi_ang(4));
//        tft_prints(0,5,"%d",get_MTi_ang(5));
        
}

void determineZone(){
    switch(hueAvg){
        case 0:
            gameZone = PINKZONE;
            strcpy(gameZoneString,"PinkStart");
            river  = 0;
            break;
        case 1:
            gameZone = LIGHTBLUEZONE;
            strcpy(gameZoneString,"BlueStart");
            river = 0;
            break;
        case 2:
            gameZone = DARKGREENZONE;
            strcpy(gameZoneString,"Darkgreen");
            river = 1;
            break;
        case 3:
            gameZone = ORANGEZONE;
            strcpy(gameZoneString,"Orange");
            river = 0;
            break;
        case 4:
            gameZone = BLUEZONE;
            strcpy(gameZoneString,"Darkblue");
            river = 1;
            break;
        case 5:
            gameZone = LIGHTGREENZONE;
            strcpy(gameZoneString,"Lightgreen");
            river = 0;
            break;
        default:
            gameZone = NOCOLOURZONE;
            strcpy(gameZoneString,"Unknown");
            
    }
}

void runUserInterface(void){
    //User Interface Section
    if(button_pressed(BUTTON_RED)){
        while(button_pressed(BUTTON_RED));
        switch(buttonRedCount){
            case 0:
                currentSlopeZone = GREENSLOPE1;
                strcpy(currentSlopeZoneString,"GREENSLOPE1");
                buttonRedCount++;
            break;
            case 1:
                currentSlopeZone = ORANGE1;
                strcpy(currentSlopeZoneString,"ORANGE1");
                buttonRedCount++;
            break;
            case 2:
                currentSlopeZone = GREENSLOPE2;
                strcpy(currentSlopeZoneString,"GREENSLOPE2");
                buttonRedCount++;    
            break;
            
            case 3:
                currentSlopeZone = ORANGE2;
                strcpy(currentSlopeZoneString,"ORANGE2");
                buttonRedCount++;     
            break;
            case 4:
                currentSlopeZone = GREENSLOPE3;
                strcpy(currentSlopeZoneString,"GREENSLOPE3");
                buttonRedCount++;  
            break;
            case 5:
                currentSlopeZone = STARTZONE;
                strcpy(currentSlopeZoneString,"STARTZONE");
                buttonRedCount = 0;    
            break;
        }
    }
    
    if(button_pressed(BUTTON_WHITE)){
        while(button_pressed(BUTTON_WHITE));
        switch(buttonWhiteCount){
            case 0:
                fullWhite = 1;
                passedRiver = 0;
                passedDownSlope = 0;
                buttonWhiteCount++;
            break;
            case 1:
                passedRiver = 1;
                fullWhite = 1;
                START_UP_play;
                buttonWhiteCount++;
            break;
            case 2:
                fullWhite = 1;
                passedRiver = 1;
                passedDownSlope = 1;
                buttonWhiteCount++;
            break;    
        }
        currentSlopeZone = FINISHEDSLOPE;
        strcpy(currentSlopeZoneString,"FINISHEDSLOPE");
    }
}






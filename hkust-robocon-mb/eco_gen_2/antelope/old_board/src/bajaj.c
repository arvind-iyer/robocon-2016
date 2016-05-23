#include "bajaj.h"
#include "string.h"

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
extern bool fullWhite;
extern bool systemOn;
extern int yaw_of_imu;
extern int pitch_of_imu;
extern int hueAvg;

float imuFactor;
int imuMovement;
int sat1;
int value1;
uint32_t encoder_revolution = 0;
char gameZoneString[12]= "UNKNOWN";
ZONE gameZone;

void update_encoder(){
    if(encoder_revolution > 3)encoder_revolution = 0;
    if(get_count(ENCODER2) > (int)SLOPE_ENCODER){
        encoder_revolution++;
        reset_encoder_2();
    }
}

void systemInit(){
    SystemCoreClockUpdate();
	led_init();			//Initiate LED
	ticks_init();		//Ticks initialization
    tft_init(PIN_ON_BOTTOM,BLACK,WHITE,RED);     //LCD Initialization
	buzzer_init();	//Initialize buzzer
    servo_init();
    button_init();
    encoder_init();
    infrared_sensor_init();
    ardu_imu_init();
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
    river  = msg.Data[0];
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
    update_encoder();
    tft_prints(0,0,"zone:%s",gameZoneString);
    for(int i = 0; i < 16 ;i++) tft_prints(i,1,"%d",sensorbar_result[i]);
    tft_prints(0,2,"y:%f",ardu_cal_ypr[0]);
    tft_prints(0,3,"il:%d ir:%d",read_infrared_sensor(INFRARED_SENSOR_LEFT),read_infrared_sensor(INFRARED_SENSOR_RIGHT));
    tft_prints(0,4,"ul:%d ur:%d", read_infrared_sensor(INFRARED_SENSOR_UPPER_LEFT),read_infrared_sensor(INFRARED_SENSOR_UPPER_RIGHT));
    tft_prints(0,5,"le:%d havg:%d",length,hueAvg);
    tft_prints(0,6,"fullwhite:%d",fullWhite);
    tft_prints(0,7,"e1:%d e2:%d",get_full_count(ENCODER1),get_full_count(ENCODER2));
    tft_prints(0,8,"state: %d",globalState);
    tft_prints(0,9,"riv:%d count:%d",river,encoder_revolution);    
}

void process_array(){
    //Seperate the marking into a 2 stage process
    for(int k = 0; k < 16; k++) {
        int el = sensorbar_result[k];
        if (el == 1) {
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
    for(int i = 0 ; i < 16 ;i++){
        if(sensorbar_result[i])
            length++;
    }
}


void goNormal(void){
    if (get_full_ticks() - lastTurn >= (int)DELAY){
        if(length > 9 && fullWhite == false && encoder_revolution > 1){
            fullWhite = true;
            ardu_cal_ypr[0] = (float)NINETY_IMU;
            globalState = NINETY;
        }
        
        else if (length > 9){
            lastMovement = SERVO_MICROS_MID;
        }

        else if (length >= 1 && length <= 5) {
            if(fullWhite){
                float factor = ((begin + end) / 2) / (float) 16;
                lastMovement = (SERVO_MICROS_LEFT) - (factor * (SERVO_MICROS_LEFT - SERVO_MICROS_RIGHT));
            }
            else{
                float factor = ((begin + end) / 2) / (float) 16;
                lastMovement = (SLOPE_TURNING_LEFT) - (factor * (SLOPE_TURNING_LEFT - SLOPE_TURNING_RIGHT));
            }
        }  
    begin = -1;
    servo_control(BAJAJ_SERVO,lastMovement);
    }
}
void goNinety(void){
    switch(CURRENT_SIDE){
        case LEFT_SIDE:
            if((int)ardu_cal_ypr[0] > -75){
                globalState = NOT_RIVER;
            }
        break;
        case RIGHT_SIDE:
            if((int)ardu_cal_ypr[0] < 90){
                globalState = NOT_RIVER;
            }
        break;
    }
    imuFactor = ardu_cal_ypr[0] / 180.0f;
    imuMovement = SERVO_MICROS_MID + (imuFactor * 450);
    servo_control(BAJAJ_SERVO,imuMovement);
}

void goUsingImu(void){
    if((get_count(ENCODER1) > 10000) && !read_infrared_sensor(RIVER_INFRARED)){
        globalState = STAGE2;
        reset_encoder_1();
    }
    imuFactor = ardu_cal_ypr[0] / 180.0f;
    imuMovement = SERVO_MICROS_MID + (imuFactor * 450);
    servo_control(BAJAJ_SERVO,imuMovement);
}

void goStraightLittleBit(void){
    servo_control(BAJAJ_SERVO,SERVO_MICROS_MID + (int)LESSER_TURNING);
    if(get_count(ENCODER1) > 6000){
        lastMovement = SERVO_MICROS_MID + (int)LESSER_TURNING;
        globalState = NOT_RIVER;
        lastMovement = SERVO_MICROS_MID + (int)LESSER_TURNING;
    }
}

void printSystemOff(void){
    tft_prints(0,0,"e1:%d e2:%d",get_full_count(ENCODER1),get_full_count(ENCODER2));
    tft_prints(0,1,"zone: %s",gameZoneString);
    for(int i = 0; i < 16 ;i++) tft_prints(i,2,"%d",sensorbar_result[i]);
    tft_prints(0,3,"river:%d",river);
    tft_prints(0,4,"calibrated:%d",ardu_imu_calibrated);
    tft_prints(0,5,"yaw:%f",ardu_cal_ypr[0]);
    tft_prints(0,6,"length:%d fw:%d",length,fullWhite);
    tft_prints(0,7,"b:%d e:%d",begin,end);
    tft_prints(0,8,"il:%d ir:%d",read_infrared_sensor(INFRARED_SENSOR_LEFT),read_infrared_sensor(INFRARED_SENSOR_RIGHT));
    tft_prints(0,9,"ul:%d ur:%d", read_infrared_sensor(INFRARED_SENSOR_UPPER_LEFT),read_infrared_sensor(INFRARED_SENSOR_UPPER_RIGHT));
}

void determineZone(){
    switch(hueAvg){
        case 0:
            gameZone = PINKZONE;
            strcpy(gameZoneString,"Start/End");
            river  = 0;
            break;
        case 1:
            gameZone = DARKGREENZONE;
            strcpy(gameZoneString,"Darkgreen");
            river = 1;
            break;
        case 2:
            gameZone = ORANGEZONE;
            strcpy(gameZoneString,"Orange");
            river = 0;
            break;
        case 3:
            gameZone = BLUEZONE;
            strcpy(gameZoneString,"River");
            river = 1;
            break;
        case 4:
            gameZone = LIGHTGREENZONE;
            strcpy(gameZoneString,"Lightgreen");
            break;
        default:
            gameZone = NOCOLOURZONE;
            strcpy(gameZoneString,"Unknown");
            
    }
}

void runUserInterface(void){
    //User Interface Section
    if(button_pressed(BUTTON_JS_DOWN))fullWhite = true;
    if(button_pressed(BUTTON_JS_LEFT))systemOn = 0;
    if(button_pressed(BUTTON_JS_RIGHT))systemOn = 1;

}







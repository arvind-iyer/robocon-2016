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
extern bool sensorIsFlipped;
extern bool fullWhite;
extern bool systemOn;
extern int yaw_of_imu;
extern int pitch_of_imu;

float imuFactor;
int imuMovement;

int sat1;
int value1;



long encoder_value = 0;

ZONE gameZone;


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
    river = msg.Data[0];
    hueAvg = msg.Data[1];
    border = msg.Data[2];
    value1 = msg.Data[3];
    sat1 = msg.Data[4];
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
    tft_prints(0,0,"zone:%d  cal:%d",gameZone,ardu_imu_calibrated);
    for(int i = 0; i < 16 ;i++) tft_prints(i,1,"%d",sensorbar_result[i]);
    tft_prints(0,2,"y:%d p:%d",yaw_of_imu,pitch_of_imu);
    tft_prints(0,3,"il:%d ir:%d",read_infrared_sensor(INFRARED_SENSOR_LEFT),read_infrared_sensor(INFRARED_SENSOR_RIGHT));
    tft_prints(0,4,"ul:%d ur:%d", read_infrared_sensor(INFRARED_SENSOR_UPPER_LEFT),read_infrared_sensor(INFRARED_SENSOR_UPPER_RIGHT));
    tft_prints(0,5,"le:%d hAvg:%d",length,hueAvg);
    tft_prints(0,6,"fullwhite:%d",fullWhite);
    tft_prints(0,7,"e1:%d e2:%d",get_full_count(ENCODER1),get_full_count(ENCODER2));
    tft_prints(0,8,"state: %d",globalState);
    tft_prints(0,9,"riv:%d",river);    
}

void process_array(){
    for(int k = 0; k < 16; k++) {
        int el = sensorbar_result[k];
        if (el == 1) {
            length++;
            if (begin == -1) begin = k; 
            else if(sensorbar_result[k + 1] == 0){
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
    if (get_full_ticks() - lastTurn >= 800){
        if(length == 0 && gameZone !=  ORANGEZONE)lastMovement = SERVO_MICROS_MID - 150;
                
        else if(length >= 11 && fullWhite == false && gameZone == ORANGEZONE && pitch_of_imu >= -1 && pitch_of_imu <= 1 ){
            lastMovement = SERVO_MICROS_RIGHT - 60;
            fullWhite = true;
            lastTurn = get_full_ticks();
        }
        
        else if (length >= 11 && fullWhite == true){
            lastMovement = SERVO_MICROS_MID;
        }

        else if (length >= 1 && length <= 7) {
            float factor = ((begin + end) / 2) / (float) 16;
            lastMovement = (SERVO_MICROS_LEFT) - (factor * (SERVO_MICROS_LEFT - SERVO_MICROS_RIGHT));
        }  
    begin = -1;
    servo_control(BAJAJ_SERVO,lastMovement);
    }
}
void goUsingImu(void){
    if((get_count(ENCODER1) > 16000) && !read_infrared_sensor(INFRARED_SENSOR_RIGHT)){
        globalState = STAGE2;
        reset_all_encoder();
    }
    yaw_of_imu = yaw_of_imu > 180 ? 180 : yaw_of_imu;
    yaw_of_imu = yaw_of_imu < -180 ? -180 : yaw_of_imu; 
    imuFactor = (float)yaw_of_imu / 180;
    imuMovement = SERVO_MICROS_MID + (imuFactor * 500);
    servo_control(BAJAJ_SERVO,imuMovement);
}

void goStraightLittleBit(void){
    servo_control(BAJAJ_SERVO,SERVO_MICROS_MID + 200);
    if(get_count(ENCODER1) > 3500){
        lastMovement = SERVO_MICROS_MID + 200;
        globalState = NOT_RIVER;
        lastMovement = SERVO_MICROS_MID + 200;
    }
}

void printSystemOff(void){
    tft_prints(0,0,"e1:%d e2:%d",get_full_count(ENCODER1),get_full_count(ENCODER2));
    tft_prints(0,1,"zone: %d fw:%d",gameZone,fullWhite);
    for(int i = 0; i < 16 ;i++) tft_prints(i,2,"%d",sensorbar_result[i]);
    tft_prints(0,3,"river:%d hue:%d",river,hueAvg);
    tft_prints(0,4,"calibrated:%d",ardu_imu_calibrated);
    tft_prints(0,5,"yaw:%f",ardu_cal_ypr[0],ardu_cal_ypr[1]);
    tft_prints(0,6,"length:%d",length);
    tft_prints(0,7,"b:%d e:%d",begin,end);
    tft_prints(0,8,"il:%d ir:%d",read_infrared_sensor(INFRARED_SENSOR_LEFT),read_infrared_sensor(INFRARED_SENSOR_RIGHT));
    tft_prints(0,9,"ul:%d ur:%d", read_infrared_sensor(INFRARED_SENSOR_UPPER_LEFT),read_infrared_sensor(INFRARED_SENSOR_UPPER_RIGHT));
}

void determineZone(){
    //Dark blue
    if(hueAvg >= 220 && hueAvg <= 240){gameZone = BLUEZONE;}
    
    //Orange
    else if(hueAvg <= 40 && hueAvg > 10){gameZone = ORANGEZONE;}
    
    //Light green
    else if(hueAvg >= 90 && hueAvg < 180){gameZone = LIGHTGREENZONE;}
    
    //Dark green
    else if(hueAvg >= 180 && hueAvg < 220){gameZone = DARKGREENZONE;}
    
    //Pink
    else if(hueAvg > 38 || hueAvg < 90){gameZone = PINKZONE;}
}






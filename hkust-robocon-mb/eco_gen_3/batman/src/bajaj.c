#include "bajaj.h"

//Values Define which will be filled after starting the program
int MAX_NINETY_TURNING;
int IMU_ANGLE1;
float NINETY_IMU;
int LESSER_TURNING;
int NINETY_TURNING;
int SLOPE_TURNING_RIGHT;
int SLOPE_TURNING_LEFT;
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
int passedOrangeBeforeDownSlope = 0;
float imuFactor;
int imuMovement;
int sat1;
int value1;
int button_count_red = 0;
int button_count_white = 0;
uint32_t encoder_revolution = 0;
char gameZoneString[12]= "UNKNOWN";
ZONE gameZone;
ZONE expectedGameZone;
SLOPEZONE currentSlopeZone;
char currentSlopeZoneString[16];


void initializeValues(void){
    if(button_pressed(BUTTON_RED)){
        while(button_pressed(BUTTON_RED));
        tft_init(PIN_ON_BOTTOM,DARKWHITE,DARK_RED,RED); 
        IMU_ANGLE1 = -35;
        NINETY_IMU = -180;
        LESSER_TURNING = -275;
        SLOPE_TURNING_RIGHT = 1850;
        SLOPE_TURNING_LEFT = 1250;
        DELAY = 1000;
        side = REDSIDE;
        infrared1 = INFRARED_SENSOR_RIGHT;
        infrared2 = INFRARED_SENSOR_LEFT;
        buttonRedCount = 0;
        NINETY_TURNING = 1750;
        currentSlopeZone = STARTZONE;
        strcpy(currentSlopeZoneString,"STARTZONE");
        systemOn = 1;
    }
    else if(button_pressed(BUTTON_WHITE)){
        while(button_pressed(BUTTON_WHITE));
        tft_init(PIN_ON_BOTTOM,DARKWHITE,DARKBLUE,RED);
        IMU_ANGLE1 = 80;
        NINETY_IMU = 180;
        LESSER_TURNING = 300;
        SLOPE_TURNING_RIGHT = 1850;
        SLOPE_TURNING_LEFT = 1250;
        DELAY = 1000;
        side = BLUESIDE;
        infrared1 = INFRARED_SENSOR_LEFT;
        infrared2 = INFRARED_SENSOR_RIGHT;
        buttonWhiteCount = 0;
        NINETY_TURNING = 1250;
        currentSlopeZone = STARTZONE;
        strcpy(currentSlopeZoneString,"STARTZONE");
        systemOn = 1;
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
    ardu_imu_init();
    //MTi_1_UART_init();

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
    tft_prints(0,9,"y:%.2f v:%.2f",ardu_cal_ypr[0],determine_velocity(ENCODER1));
    tft_prints(0,3,"il:%d ir:%d",read_infrared_sensor(INFRARED_SENSOR_LEFT),read_infrared_sensor(INFRARED_SENSOR_RIGHT));
    tft_prints(0,4,"ul:%d ur:%d", read_infrared_sensor(INFRARED_SENSOR_UPPER_LEFT),read_infrared_sensor(INFRARED_SENSOR_UPPER_RIGHT));
    tft_prints(0,5,"le:%d state:%d",length,globalState);
    tft_prints(0,6,"fw:%d, river:%d",fullWhite,river);
    tft_prints(0,7,"v: %.2f e:%d", determine_velocity(ENCODER1),get_minimize_count(ENCODER1));
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
    if (get_full_ticks() - lastTurn >= (int)DELAY){            
        if (length >= 1 && length <= 16) {
            if (fullWhite && !passedRiver){
                if ((((begin + end)/ 2) - 4) < 0) {
                    float factor = 0;
                    lastMovement = (SERVO_MICROS_LEFT ) - (factor * (SERVO_MICROS_LEFT - SERVO_MICROS_RIGHT));
                }
                else{
                    float factor = (((begin + end)/ 2) - 4) / (float) 16;
                    lastMovement = (SERVO_MICROS_LEFT) - (factor * (SERVO_MICROS_LEFT - SERVO_MICROS_RIGHT));
                }
            }
            else if(passedDownSlope){
                if ((((begin + end)/ 2) + 3) > 16) {
                    lastMovement = SERVO_MICROS_RIGHT;
                }
                else{
                    float factor = (((begin + end)/ 2) + 3) / (float) 16;
                    lastMovement = (SERVO_MICROS_LEFT) - (factor * (SERVO_MICROS_LEFT - SERVO_MICROS_RIGHT));
                }   
            }
            else if(fullWhite && passedRiver){
                float factor = ((begin + end)/ 2) / (float) 16;
                lastMovement = (SERVO_MICROS_LEFT) - (factor * (SERVO_MICROS_LEFT - SERVO_MICROS_RIGHT));
            }
            else{
                float factor = ((begin + end)/ 2) / (float) 16;
                lastMovement = (SLOPE_TURNING_LEFT) - (factor * (SLOPE_TURNING_LEFT - SLOPE_TURNING_RIGHT));
            }
        }
        if(passedRiver && fullWhite && gameZone == ORANGEZONE){
            passedOrangeBeforeDownSlope = 1;
        }
        if(gameZone == LIGHTGREENZONE && passedOrangeBeforeDownSlope){
            passedDownSlope = 1;
        }
    servo_control(BAJAJ_SERVO,lastMovement);
    }
}
void goNinety(void){
    switch(side){
        case REDSIDE:
            if((int)ardu_cal_ypr[0] > -85){
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
    lastMovement = NINETY_TURNING  + ((int)determine_velocity(ENCODER1) * 20);
    servo_control(BAJAJ_SERVO,lastMovement);
}
/*
bool first_island_detected = false;
bool first_island_passed = false;
bool second_island_detected = false;
bool second_island_passed = false;
void goRiver(void) {
	if(!first_island_detected && !read_infrared_sensor(infrared1)) {
		first_island_detected = true;
	}
	if(first_island_detected && !first_island_passed && read_infrared_sensor(infrared1)) {
		first_island_passed = true;
		servo_control(BAJAJ_SERVO, SERVO_MICROS_MID - 300);	
	}
	if(read_infrared_sensor(infrared2)) {
		servo_control(BAJAJ_SERVO, SERVO_MICROS_MID + 400);
	}
	if(first_island_passed && read_infrared_sensor(infrared1)) {
		servo_control(BAJAJ_SERVO, SERVO_MICROS_MID - 200);
		globalState = STAGE2;
	}
	if((get_count(ENCODER1) > 24000) && !read_infrared_sensor(infrared2)) {
		servo_control(BAJAJ_SERVO, SERVO_MICROS_MID + 80);
		globalState = STAGE2;
	}		
}	
*/


void goUsingImu(void){
    imuFactor = ardu_cal_ypr[0] / 180.0f;
    lastMovement = imuMovement = SERVO_MICROS_MID - (imuFactor * 400);
    servo_control(BAJAJ_SERVO,lastMovement);
    
    //Stopping condition
    if((get_count(ENCODER1) > 20000) && !read_infrared_sensor(infrared2)){
        reset_encoder_1();
        globalState = STAGE2;
    }
}


int getLineCenter(void) {
	int center = 0, num_samples = 0; 
	for(int i = 0; i < 16; i++){
		if(sensorbar_result[i]) {
			center += i;
			num_samples++;
		}
	}
	center = 0.5+ (center / num_samples);
	center -= 8;
	return -center;
}

void goStraightLittleBit(void){
    switch(side){
        case BLUESIDE:
            lastMovement = SERVO_MICROS_MID + (int)LESSER_TURNING + (int)determine_velocity(ENCODER1) * 15;
            break;
        case REDSIDE:
            lastMovement = SERVO_MICROS_MID + (int)LESSER_TURNING - (int)(determine_velocity(ENCODER1) * 10);
            break;
    } 
    servo_control(BAJAJ_SERVO, lastMovement);
    //Stopping condition
    if(get_count(ENCODER1) > 3000){
        passedRiver = 1;
        globalState = NOT_RIVER;
    }
}

void printSystemOff(void){
      tft_prints(0,0,"PRESS RED / WHITE");
      tft_prints(0,1,"e1:%d e1f:%d",get_minimize_count(ENCODER1),get_full_count(ENCODER1));
      tft_prints(0,2,"velo:%.2f",determine_velocity(ENCODER1));
      tft_prints(0,3,"zone: %s",gameZoneString);
      for(int i = 0; i < 16 ;i++) tft_prints(i,4,"%d",sensorbar_result[i]);
      tft_prints(0,5,"calibrated:%d",ardu_imu_calibrated);
      tft_prints(0,6,"yaw:%.2f",ardu_cal_ypr[0]);
      tft_prints(0,7,"length:%d fw:%d",length,fullWhite);
      tft_prints(0,8,"il:%d ir:%d",read_infrared_sensor(INFRARED_SENSOR_LEFT),read_infrared_sensor(INFRARED_SENSOR_RIGHT));
      tft_prints(0,9,"ul:%d ur:%d", read_infrared_sensor(INFRARED_SENSOR_UPPER_LEFT),read_infrared_sensor(INFRARED_SENSOR_UPPER_RIGHT));
}

void determineZone(){
    switch(hueAvg){
        case 0:
            switch(side){
                case REDSIDE:
                    gameZone = PINKZONE;
                    strcpy(gameZoneString,"PinkStart");
                    break;
                case BLUESIDE:
                    gameZone = LIGHTBLUEZONE;
                    strcpy(gameZoneString,"BlueStart");
                    break;
            }
            river  = 0;
            break;
        case 1:
            switch(side){
                case REDSIDE:
                    gameZone = PINKZONE;
                    strcpy(gameZoneString,"PinkStart");
                    break;
                case BLUESIDE:
                    gameZone = LIGHTBLUEZONE;
                    strcpy(gameZoneString,"BlueStart");
                    break;
            }
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
        button_count_red++;
        if(button_count_red > 100){
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
            button_count_red = 0;
        }
    }
    
    if(button_pressed(BUTTON_WHITE)){
        button_count_white++;
        if(button_count_white > 100){
            while(button_pressed(BUTTON_WHITE));
            switch(buttonWhiteCount){
                case 0:
                    fullWhite = 1;
                    passedRiver = 0;
                    passedDownSlope = 0;
                    strcpy(currentSlopeZoneString,"FINISHEDSLOPE");
                    buttonWhiteCount++;
                break;
                case 1:
                    passedRiver = 1;
                    fullWhite = 1;
                    strcpy(currentSlopeZoneString,"FINISHEDRIVER");
                    buttonWhiteCount++;
                break;
                case 2:
                    fullWhite = 1;
                    passedRiver = 1;
                    passedDownSlope = 1;
                    strcpy(currentSlopeZoneString,"PASSDOWNSLOPE");
                    buttonWhiteCount++;
                break;
                case 3:
                    fullWhite = 1;
                    passedRiver = 0;
                    passedDownSlope = 0;                
                    strcpy(currentSlopeZoneString,"FINISHEDSLOPE");
                    buttonWhiteCount = 0;
                break;    
            }
            currentSlopeZone = FINISHEDSLOPE;
            button_count_white = 0;
        }
    }
}

float determine_imu_angle(void){
    float incremental_value = 0;
    switch(side){
        case BLUESIDE:
            incremental_value = determine_velocity(ENCODER1) > 2 ? (float)IMU_ANGLE1 + (determine_velocity(ENCODER1) * 3) : IMU_ANGLE1;
            break;
        case REDSIDE:
					if (determine_velocity(ENCODER1) <= 2){
						incremental_value = -20;
					}
					else{
						incremental_value = IMU_ANGLE1;
					
					}
            //incremental_value = determine_velocity(ENCODER1) > 2 ? (float)IMU_ANGLE1 - (determine_velocity(ENCODER1) * 3) : IMU_ANGLE1;
            break;
    }        
    return IMU_ANGLE1;
}







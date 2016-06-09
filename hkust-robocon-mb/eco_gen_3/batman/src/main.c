#include "main.h"

//Extern variables, sorry if it is a mess,yolo global variables
extern int MAX_NINETY_TURNING;
extern int IMU_ANGLE1;
extern float NINETY_IMU;
extern int LESSER_TURNING;
extern int SLOPE_TURNING_RIGHT;
extern int SLOPE_TURNING_LEFT;
extern int DELAY;
extern GAMESIDE side;
extern INFRARED_SENSOR infrared1;
extern INFRARED_SENSOR infrared2;
extern SLOPEZONE currentSlopeZone;
extern ZONE gameZone;


bool sensorIsFlipped = true;
u8 data1[8];
u8 data2[8];
u8 sensorbar_result[16];
u8 river;
int hueAvg;
u8 border;
u8 globalState = NOT_RIVER;
int begin = -1;
int end = 0;
int length = 0;
int lastTurn = 0;
float factor = 0;
int fullWhite = 0;
int systemOn = 0;
int yaw_of_imu = 0;
int pitch_of_imu = 0;
int lastMovement = SERVO_MICROS_MID;
extern int passedRiver;
extern int passedDownSlope;
extern char currentSlopeZoneString[10];

int main(void) {
    //Initialization of all hardware
    systemInit();
    u32 ticks_ms_img = 0;
    bool songIsPlayed = false;
    bool startSong = false;
    bool cali = false;
    while (1) {
        if(ticks_ms_img != get_ticks()){
            buzzer_check();
            ticks_ms_img = get_ticks();
            tft_clear();
            fill_sensorbar_array();
            process_array();
            determineZone();
            if(get_ticks() % 25 == 0)ardu_imu_value_update();
            //Play song when IMU is calibrated
              if(ardu_imu_calibrated){
                cali = true;
              }
              if(cali && !songIsPlayed){
                MARIO_START;
                songIsPlayed = true;
              }
            
            switch(systemOn){
                case ON:
                    //Emergency turning system
                    if(read_infrared_sensor(INFRARED_SENSOR_UPPER_LEFT)){
                        if(!fullWhite)servo_control(BAJAJ_SERVO,SERVO_MICROS_RIGHT + 240);
                        else servo_control(BAJAJ_SERVO, SERVO_MICROS_RIGHT);
                    }
                    else if(read_infrared_sensor(INFRARED_SENSOR_UPPER_RIGHT)){
                        if(!fullWhite)servo_control(BAJAJ_SERVO, SERVO_MICROS_LEFT - 240);
                        else servo_control(BAJAJ_SERVO, SERVO_MICROS_LEFT);
                    }
                    
                    //Normal working state
                    else{
                        switch(globalState){
                            case NOT_RIVER:
                                switch(currentSlopeZone){
                                    case STARTZONE:
                                        goNormal();
                                        if(!startSong){
                                            START_UP_play;
                                            startSong = true;
                                        }                                       
                                        if(gameZone == DARKGREENZONE){
                                            currentSlopeZone = GREENSLOPE1;
                                            strcpy(currentSlopeZoneString,"GREENSLOPE1");
                                        }   
                                    break;
                                    case GREENSLOPE1:
                                        goNormal();
                                        if(gameZone == ORANGEZONE){
                                            currentSlopeZone = ORANGE1;
                                            strcpy(currentSlopeZoneString,"ORANGE1");
                                        }   
                                    break;
                                    case ORANGE1:
                                        goNormal();
                                        if(gameZone == DARKGREENZONE){
                                            currentSlopeZone = GREENSLOPE2;
                                            strcpy(currentSlopeZoneString,"GREENSLOPE2");
                                        }    
                                    break;
                                    case GREENSLOPE2:
                                        goNormal();
                                        if(gameZone == ORANGEZONE){
                                            currentSlopeZone = ORANGE2;
                                            strcpy(currentSlopeZoneString,"ORANGE2");
                                        }    
                                    break;
                                    case ORANGE2:
                                        goNormal();
                                        if(gameZone == DARKGREENZONE){
                                            currentSlopeZone = GREENSLOPE3;
                                            strcpy(currentSlopeZoneString,"GREENSLOPE3");
                                        }
                                    break;
                                    case GREENSLOPE3:
                                        goNormal();
                                        if(gameZone == ORANGEZONE){
                                            currentSlopeZone = FINISHEDSLOPE;
                                            strcpy(currentSlopeZoneString,"FINISHEDSLOPE");
                                        }    
                                    break;
                                    case FINISHEDSLOPE:
                                        switch(fullWhite){
                                            case 0:
                                                ardu_cal_ypr[0] = (float)NINETY_IMU;
                                                globalState = NINETY;
                                            break;
                                            case 1:
                                                if((river || gameZone == LIGHTGREENZONE) && fullWhite && !passedRiver)
                                                    {
                                                        START_UP_play;
                                                        servo_control(BAJAJ_SERVO, SERVO_MICROS_MID - 150);
                                                        globalState = STAGE3;
                                                    }
                                                else 
                                                    goNormal();
                                            break;
                                        }
                                    break;                                           
                                }
                            break;
                            case NINETY:
                                goNinety();
                            break;
                            case STAGE1:
                                goUsingImu(); //Imu is bae, thx Rex!
                            break;
                            case STAGE2:
                                goStraightLittleBit(); //Prevent it from falling down
                            break;
                            case STAGE3: //Right before locking the angle with IMU
                                if(!read_infrared_sensor(infrared1)){
                                    ardu_cal_ypr[0] = determine_imu_angle();
                                    reset_encoder_1();
                                    globalState = STAGE1;
                                }
                            break;
                            case STAGE4: //End game, make it turn right / left for the hybrid
                                if(determine_velocity(ENCODER1) < (float)1.0){
                                    switch(side){
                                        case REDSIDE:
                                            servo_control(BAJAJ_SERVO,2300);
                                        break;
                                        case BLUESIDE:
                                            servo_control(BAJAJ_SERVO,900);
                                        break;
                                    }
                                }
                            break;
                        }
                    }
                    print_data(); //Print every data in the on(servo is active) system
                    runUserInterface(); //Button functions
                    break;
               
                case OFF:
                    servo_control(BAJAJ_SERVO,SERVO_MICROS_MID);
                    initializeValues();
                    printSystemOff(); //Print every data in off(servo locked in 90 degrees) system
                    break;
              }
              length = 0;
            
              //Button functions are run by this
              tft_update();
        }
    }
}


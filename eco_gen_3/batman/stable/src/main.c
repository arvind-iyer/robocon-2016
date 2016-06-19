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
bool ecoFinish = false;
u8 data1[8];
u8 data2[8];
u8 sensorbar_result[16];
u8 river;
int hueAvg;
u8 border;
u8 globalState = NORMAL;
int begin = -1;
int eco_finish_timestamp = 0;
int end = 0;
int length = 0;
int lastTurn = 0;
float factor = 0;
int fullWhite = 0;
int systemOn = 0;
int yaw_of_imu = 0;
int pitch_of_imu = 0;
int lastMovement = SERVO_MICROS_MID;
int time1 = 0;
int time2 = 0;
float angle_after_ninety = 0.0;
float angle_enter_river = 0.0;
bool done_turning = false;

extern int passedRiver;
extern int passedDownSlope;
extern int passedOrangeBeforeDownSlope;
extern char currentSlopeZoneString[10];
extern char globalStateString[16];
extern int ENTER_RIVER_ENCODER;

bool imu_fucked_up = false;


int main(void) {
    //Initialization of all hardware
    systemInit();
    u32 ticks_ms_img = 0;
    bool songIsPlayed = false;
    bool startSong = false;
    bool final_music = false;
    bool cali = false;
    int enter_time_stamp;
    reset_encoder_1();
	while (1) {
        if(ticks_ms_img != get_ticks()){
            buzzer_check();
            ticks_ms_img = get_ticks();
            tft_clear();
            fill_sensorbar_array();
            process_array();
            determineZone();
            if((get_ticks() % 25 == 0) && !imu_fucked_up)
                ardu_imu_value_update();
            //Play song when IMU is calibrated
            if(ardu_imu_calibrated)
                cali = true;
            if(cali && !songIsPlayed){
                MARIO_START;
                songIsPlayed = true;
            }
            switch(systemOn){
                case ON:
                    //Emergency turning system
                    if(read_infrared_sensor(INFRARED_SENSOR_UPPER_LEFT) && passedRiver && (globalState != DOWN_SLOPE)){
                       servo_control(BAJAJ_SERVO,SERVO_MICROS_RIGHT - 150);
                    }
                    else if(read_infrared_sensor(INFRARED_SENSOR_UPPER_RIGHT) && passedRiver && (globalState != DOWN_SLOPE)){
                       servo_control(BAJAJ_SERVO, SERVO_MICROS_LEFT + 150);
                    }
                    //Normal working state
                    else{
                        switch(globalState){
                            case NORMAL:
                                switch(currentSlopeZone){
                                    case STARTZONE:
                                        if(!startSong){
                                            START_UP_play;
                                            startSong = true;
                                        }                                       
                                        if(gameZone == DARKGREENZONE && (get_minimize_count(ENCODER1) > 10)){
                                            reset_encoder_1();
											START_UP_play;
                                            currentSlopeZone = GREENSLOPE1;
                                            strcpy(currentSlopeZoneString,"GREENSLOPE1");
                                        } 
                                        else
                                            goNormal();
                                    break;
                                    case GREENSLOPE1:
                                        if((gameZone == ORANGEZONE || gameZone == PINKZONE) && (get_minimize_count(ENCODER1) > 4)){
                                            reset_encoder_1();
                                            START_UP_play;
                                            currentSlopeZone = ORANGE1;
                                            strcpy(currentSlopeZoneString,"ORANGE1");
                                        }  
                                        else
                                            goNormal();
                                            
                                    break;
                                    case ORANGE1:
                                        if(gameZone == DARKGREENZONE && (get_minimize_count(ENCODER1) > 4)){
                                            reset_encoder_1();
                                            START_UP_play;
                                            currentSlopeZone = GREENSLOPE2;
                                            strcpy(currentSlopeZoneString,"GREENSLOPE2");
                                        } 
                                        else
                                            goNormal();
                                    break;
                                    case GREENSLOPE2: 
                                        if((gameZone == ORANGEZONE || gameZone == PINKZONE) && (get_minimize_count(ENCODER1) > 4)){
                                            reset_encoder_1();
                                            START_UP_play;
                                            currentSlopeZone = ORANGE2;
                                            strcpy(currentSlopeZoneString,"ORANGE2");
                                        }
                                        else 
                                            goNormal();                                        
                                    break;
                                    case ORANGE2:
                                        if(gameZone == DARKGREENZONE && (get_minimize_count(ENCODER1) > 4)){
                                            reset_encoder_1();
											START_UP_play;
                                            currentSlopeZone = GREENSLOPE3;
                                            strcpy(currentSlopeZoneString,"GREENSLOPE3");
                                        }
                                        else
                                            goNormal();                                            
                                            
                                    break;
                                    case GREENSLOPE3:
                                        if((gameZone == ORANGEZONE || gameZone == PINKZONE) && (get_minimize_count(ENCODER1) > 4)){
                                            currentSlopeZone = FINISHEDSLOPE;
											START_UP_play;
                                            strcpy(currentSlopeZoneString,"FINISHEDSLOPE");
                                        }   
                                        else
                                            goNormal();
                                    break;
                                    case FINISHEDSLOPE:
                                        switch(fullWhite){
                                            case 0:
                                                ardu_cal_ypr[0] = (float)NINETY_IMU;
                                                strcpy(globalStateString,"NINETY DEGREE");
                                                globalState = NINETY;
                                            break;
                                            case 1:
                                                if((river) && !passedRiver)
                                                    {
                                                        START_UP_play;
                                                        reset_encoder_1();
                                                        strcpy(globalStateString,"ENTER_RIVER");  
                                                        time1 = get_full_ticks();
                                                        angle_enter_river = ardu_cal_ypr[0];
                                                        globalState = ENTER_RIVER;
                                                    } 
                                                else if(passedDownSlope && (get_minimize_count(ENCODER1)> 10)){
                                                    START_UP_play;
                                                    strcpy(globalStateString,"DOWN SLOPE");
                                                    reset_encoder_1();
                                                    globalState = DOWN_SLOPE;
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
                            case EXIT_RIVER:
                                goStraightLittleBit(); //Prevent it from falling down
                            break;
                            case ENTER_RIVER: //Right before locking the angle with IMU
                                //Stopping condition:
                                if(get_count(ENCODER1) > ENTER_RIVER_ENCODER){
                                    switch(side){
                                        case REDSIDE:
											servo_control(BAJAJ_SERVO,SERVO_MICROS_MID + 350);
                                        break;
                                        case BLUESIDE:
                                            servo_control(BAJAJ_SERVO,SERVO_MICROS_MID - 350);
                                        break;
                                    }
                                    strcpy(globalStateString,"ESCAPEISLAND");
                                    START_UP_play;
                                    reset_encoder_1();
                                    time2 = get_full_ticks();
                                    globalState = RIVERING; 
                                }
                                else
                                    goNormal();
                            break;
                            case RIVERING:
                                escapeFirstIsland();
                            break;
                            case RIVERING2:
                                scanRiver();
                            break;
                            case DOWN_SLOPE: //End game, make it turn extreme right / left for the hybrid to grip propeller
                                if(get_minimize_count(ENCODER1) > 10 && (gameZone != LIGHTGREENZONE)){
                                    enter_time_stamp = get_full_ticks();
                                    strcpy(globalStateString,"FINISH GAME");
                                    globalState = FINISH;
                                }
                                else
                                    goNormal();
                            break;
                            case FINISH:
                                //Lock the servo angle
                                if((get_full_ticks() - enter_time_stamp) > (int)DELAY){
                                    if(!final_music){
                                        final_music = true;
                                        START_UP_play;
                                    }
                                    switch(side){
                                        case REDSIDE:
                                            servo_control(BAJAJ_SERVO,SERVO_MICROS_MID + 700);
                                        break;
                                        case BLUESIDE:
                                            servo_control(BAJAJ_SERVO, SERVO_MICROS_MID - 700);
                                        break;
                                    }
                                }
                                else
                                    goNormal();
                            break;          
                        }
                    }
                    print_data(); //Print every data in the on(servo is active) system
                    tft_prints(0,9,"imufucked: %d",imu_fucked_up);
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


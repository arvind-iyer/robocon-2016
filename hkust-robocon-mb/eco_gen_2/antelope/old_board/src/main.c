#include "main.h"

//Extern variables, sorry if it is a mess,yolo global variables
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

int main(void) {
    //Initialization of all hardware
    systemInit();
    u32 ticks_ms_img = 0;
    bool songIsPlayed = false;
    bool cali = false;
    while (1) {
        if(ticks_ms_img != get_ticks()){
            ticks_ms_img = get_ticks();
            tft_clear();
            fill_sensorbar_array();
            process_array();
            if(get_ticks() % 800 == 0)determineZone();
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
//                    if(read_infrared_sensor(INFRARED_SENSOR_UPPER_LEFT)){
//                        if(!fullWhite)servo_control(BAJAJ_SERVO,SERVO_MICROS_RIGHT + 50);
//                        else servo_control(BAJAJ_SERVO, SERVO_MICROS_RIGHT);
//                    }
//                    else if(read_infrared_sensor(INFRARED_SENSOR_UPPER_RIGHT)){
//                        if(!fullWhite)servo_control(BAJAJ_SERVO, SERVO_MICROS_LEFT);
//                        else servo_control(BAJAJ_SERVO, SERVO_MICROS_LEFT - 50);
//                    }
                    
                    //Normal working state
                    //else{
                        switch(globalState){
                            case NOT_RIVER:
                                goNormal();
                                if(river && !read_infrared_sensor(RIVER_INFRARED) && (fullWhite == 1))
                                {
                                    reset_encoder_1();
                                    ardu_cal_ypr[0] = (float)IMU_ANGLE;
                                    globalState = STAGE1;
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
                        }
                    //}
                    print_data(); //Print every data in the on system
                    break;
                    
                //Press down joystick if you want to turn it on
                case OFF:
                    servo_control(BAJAJ_SERVO,SERVO_MICROS_MID);
                    printSystemOff(); //Print every data in off system
                    break;
            }
            length = 0;
            
            //Button functions are run by this
            runUserInterface();
            buzzer_check();
            tft_update();
        }
    }
}


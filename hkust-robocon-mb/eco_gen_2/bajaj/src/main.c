#include "main.h"

//extern variables
u8 data1[8];
u8 data2[8];
u8 sensorbar_result[16];
u8 river;
u8 hueAvg;
u8 border;
u8 globalState = NOT_RIVER;
int begin = -1;
int end = 0;
int length = 0;
int lastMovement = SERVO_MICROS_MID + 200;
int lastTurn = 0;
float factor = 0;
bool inBlue = false;
bool sensorIsFlipped = false;
bool fullWhite = false;
int yaw_of_imu;


int main(void) {
    systemInit();
    while (true) {
        tft_clear();
        if(get_ticks() % 10){
            fill_sensorbar_array();
            process_array();
        }
        //Control System
        if(ardu_imu_calibrated)yaw_of_imu = (int)ardu_cal_ypr[0];
        //Emergency override with left and right IR
        if(read_infrared_sensor(INFRARED_SENSOR_UPPER_LEFT))servo_control(SERVO1, SERVO_MICROS_RIGHT);
        else if(read_infrared_sensor(INFRARED_SENSOR_UPPER_RIGHT))servo_control(SERVO1, SERVO_MICROS_LEFT);
        //Main control
        else{
            switch(globalState){
                case NOT_RIVER:
                    goNormal();
                    if(river && !read_infrared_sensor(INFRARED_SENSOR_RIGHT) && fullWhite)
                    {
                        reset_all_encoder();
                        ardu_cal_ypr[0] =  -120;
                        globalState = STAGE1;
                    }
                break;
                case STAGE1:
                    goUsingImu(); //Imu is bae, thx Rex!
                break;
                case STAGE2:
                    goStraightLittleBit(); //Prevent it from falling down
                break;
            }
        }
        print_data(); //Print every data in the on system
        tft_update(); 
        length = 0;
        if(get_ticks() % 20 == 0)ardu_imu_value_update();
    }
    return 0;
}
	
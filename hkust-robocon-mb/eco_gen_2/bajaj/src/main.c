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
int lastMovement = SERVO_MICROS_MID + 400;
int lastTurn = 0;
float factor = 0;
bool inBlue = false;
bool sensorIsFlipped = false;
bool fullWhite = false;
u8 systemOn = 0;

int yaw_of_imu = 0;


int main(void) {
    systemInit();
    ardu_imu_init();
    while (1) {
        tft_clear();
        if(get_full_ticks() % 20 == 0)ardu_imu_value_update();
        //Initial processing and shit
        fill_sensorbar_array();
        process_array();
        switch(systemOn){
            case ON:
                if(ardu_imu_calibrated) yaw_of_imu = (int)ardu_cal_ypr[0];
                switch(globalState){
                    case NOT_RIVER:
                        goNormal();
                        if(river && !read_infrared_sensor(INFRARED_SENSOR_2))
                        {
                            reset_all_encoder();
                            yaw_of_imu = ardu_cal_ypr[0] = -70;
                            globalState = STAGE1;
                        }
                        if(!river)inBlue = false;
                    break;
                    case STAGE1:
                        goUsingImu(); //Imu is bae, thx Rex!
                    break;
                    case STAGE2:
                        goStraightLittleBit(); //Prevent it from falling down
                    break;
                }
                print_data(); //Print every data in the on system
                break;
            case OFF:
                servo_control(SERVO1,SERVO_MICROS_MID);
                printSystemOff(); //Print every data in off system
                break;
        }
        length = 0;
        if(button_pressed(BUTTON_JS_UP))fullWhite = true;
        if(button_pressed(BUTTON_JS_DOWN))fullWhite = false;
        if(button_pressed(BUTTON_JS_LEFT))systemOn = false;
        if(button_pressed(BUTTON_JS_RIGHT)){
            systemOn = true;
            inBlue = false;
            globalState = NOT_RIVER;
        }
        tft_update();
    }
return 0;
}
	
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
int lastTurn = 0;
float factor = 0;
bool sensorIsFlipped = true;
int fullWhite = 0;
int systemOn = 0;
int yaw_of_imu = 0;
int pitch_of_imu = 0;
int lastMovement = SERVO_MICROS_MID;

extern u32 linear_ccd_buffer1[128];
extern u32 linear_ccd_buffer2[128];
extern ZONE gameZone;

int main(void) {
    systemInit();

    while (1) {
        tft_clear();
        if(get_ticks() % 20 == 0)
            {
                ardu_imu_value_update();
            }
        //Initial processing and shit
        fill_sensorbar_array();
        process_array();
        if(get_ticks() % 800 == 0)determineZone();
        switch(systemOn){
            case ON:
                if(ardu_imu_calibrated){
                    yaw_of_imu = (int)ardu_cal_ypr[0];
                    pitch_of_imu = (int)ardu_cal_ypr[1];
                }
                if(read_infrared_sensor(INFRARED_SENSOR_UPPER_LEFT)){
                    if(!fullWhite)servo_control(BAJAJ_SERVO,SERVO_MICROS_RIGHT + 250);
                    else servo_control(BAJAJ_SERVO, SERVO_MICROS_RIGHT);
                }
                else if(read_infrared_sensor(INFRARED_SENSOR_UPPER_RIGHT)){
                    if(!fullWhite)servo_control(BAJAJ_SERVO, SERVO_MICROS_LEFT);
                    else servo_control(BAJAJ_SERVO, SERVO_MICROS_LEFT - 250);
                }
                else{
                    switch(globalState){
                        case NOT_RIVER:
                            goNormal();
                            if(river && !read_infrared_sensor(INFRARED_SENSOR_RIGHT) && (fullWhite == 1))
                            {
                                reset_all_encoder();
                                yaw_of_imu = ardu_cal_ypr[0] = -120;
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
                break;
            case OFF:
                servo_control(BAJAJ_SERVO,SERVO_MICROS_MID);
                printSystemOff(); //Print every data in off system
                break;
        }
        length = 0;
        
        //Button pressing section
        if(button_pressed(BUTTON_JS_DOWN))fullWhite = true;
        if(button_pressed(BUTTON_JS_LEFT))systemOn = 0;
        if(button_pressed(BUTTON_JS_RIGHT))systemOn = 1;
        tft_update();
    }
return 0;
}
	
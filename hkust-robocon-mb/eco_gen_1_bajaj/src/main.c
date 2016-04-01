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
int lastMovement = SERVO_MICROS_MID;
int lastTurn = 0;
float factor = 0;
bool inBlue = false;
bool sensorIsFlipped = false;
bool fullWhite = false;
u8 systemOn = 0;

int main(void) {
    systemInit();

    while (1) {
        tft_clear();

        //Initial processing and shit
        fill_sensorbar_array();

        process_array();
        print_data();
        switch(systemOn){
            case ON:
                switch(globalState){
                    case NOT_RIVER:
                        goNormal();
                        if(river && !read_infrared_sensor(INFRARED_SENSOR_2))inBlue = true;
                        if(!river)inBlue = false;
                    break;

                    case STAGE1:
                        if(!river)globalState = NOT_RIVER;
                        goFindWall();
                    break;
                    
                    case STAGE2:
                        if(!river)globalState = NOT_RIVER;
                        goStraightYolo(); //Yolo baby
                    break;
                }
                break;
            case OFF:
                servo_control(SERVO1,SERVO_MICROS_MID);
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
	
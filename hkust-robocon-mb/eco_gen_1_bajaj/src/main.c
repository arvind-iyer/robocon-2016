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

int main(void) {
systemInit();

    while (1) {
        tft_clear();

        //Initial processing and shit
        fill_sensorbar_array();

        process_array();

        print_data();

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
        length = 0;
        tft_update();
    }
return 0;
}
	
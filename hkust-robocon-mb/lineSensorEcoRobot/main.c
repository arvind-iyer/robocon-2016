#include "main.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>



int main() {
	tft_init(2, BLACK, WHITE, GREEN);
	init();
    uart_init(COM3,115200);
    u8 memoryState = 0;

	long lastTick = get_ms_ticks();
    
	
	
	while (1) {
        //GPIO_SetBits(GPIOA,GPIO_Pin_9);
        if (get_ms_ticks() % 40 == 20) {
            //8 cases:
            //White : 0, Dark : 1
            //Case 1:
            if(readLineSensor(SENSOR_L) == 0 && readLineSensor(SENSOR_M) == 0 && readLineSensor(SENSOR_R) == 0){
                //All white?
            }
            //Case 2:
            if(readLineSensor(SENSOR_L) == 0 && readLineSensor(SENSOR_M) == 0 && readLineSensor(SENSOR_R) == 1){
                //Turn left 90 degrees
                memoryState = 0;
            }
            //Case 3:
            if(readLineSensor(SENSOR_L) == 0 && readLineSensor(SENSOR_M) == 1 && readLineSensor(SENSOR_R) == 0){
                //White black white?
            }
            //Case 4:
            if(readLineSensor(SENSOR_L) == 0 && readLineSensor(SENSOR_M) == 1 && readLineSensor(SENSOR_R) == 1){
                //Turn left
                
            }
            //Case 5:
            if(readLineSensor(SENSOR_L) == 1 && readLineSensor(SENSOR_M) == 0 && readLineSensor(SENSOR_R) == 0){
                //Turn right 90 degrees
                memoryState = 1;
            }
            //Case 6:
            if(readLineSensor(SENSOR_L) == 1 && readLineSensor(SENSOR_M) == 0 && readLineSensor(SENSOR_R) == 1){
                //Go straight
                servo_control(3000);
            }
            //Case 7:
            if(readLineSensor(SENSOR_L) == 1 && readLineSensor(SENSOR_M) == 1 && readLineSensor(SENSOR_R) == 0){
                //Turn right
                
            }
            //Case 8:
            if(readLineSensor(SENSOR_L) == 1 && readLineSensor(SENSOR_M) == 1 && readLineSensor(SENSOR_R) == 1){
                //All black
                servo_control(memoryState == 0 ? 2000 : 4000);
            }
            
           tft_prints(0,0,"Left: %d",readLineSensor(SENSOR_L));
           tft_prints(0,1,"Middle: %d",readLineSensor(SENSOR_M));
           tft_prints(0,2,"Right: %d",readLineSensor(SENSOR_R));
                
            
          
            
            
 
           
        }

	return 0;
    }
}
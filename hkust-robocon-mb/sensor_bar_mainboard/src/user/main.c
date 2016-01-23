#include "main.h"

u16 data1[8];
u16 data2[8];
u16 sensor_output[16];

void receive(CanRxMsg msg)
{
	for(int i = 0; i < 8 ;i++){
        data1[i] = msg.Data[i];
    }
}

void receive2(CanRxMsg msg){
    for(int i = 0; i < 8 ; i++){
        data2[i] = msg.Data[i]; 
    }
}

void fill_array(){
    for(int i = 0; i < 8; i++){
        sensor_output[i] = data2[7-i];
    }
    for(int i = 0; i < 8; i++){
        sensor_output[8+i] = data1[7-i];
    }
}

void print_array(){
    tft_prints(0,0,"Sensor output");
    for(int i = 0; i < 16 ;i++){
        
        tft_prints(i,1,"%d",sensor_output[i]);
    }


}


int main(void) 
{
	u16 val = SERVO_MED;
	
	tft_init(0,BLACK,WHITE,WHITE);
	can_init();
    servo_init();
    ticks_init();
	can_rx_init();
	can_rx_add_filter(0x0C5,CAN_RX_MASK_EXACT,receive);
    can_rx_add_filter(0x0C6,CAN_RX_MASK_EXACT,receive2);
    
    u16 ticks_ms_img = 0;
    u16 averageX = 0;
    u16 count = 0;
	while(1)    
	{
       if(get_ticks() != ticks_ms_img){ 
           ticks_ms_img = get_ticks();
           fill_array();
           print_array();
           tft_update();
       }
       
            //count average x position
            for(int i = 0;i < 16 ;i++){
                if(sensor_output[i]){
                    averageX += i;
                    count++;
                }
            }
            averageX/=count;
            tft_prints(0,2,"Average X: %d",averageX);
            /*
            #define SERVO_MIN 700
            #define SERVO_MED 1550
            #define SERVO_MAX 2400           
            */
            // 7 ama 8 gerak lurus
            if((averageX > 5) && (averageX < 11)){
                tft_prints(0,3,"Go straight");
                servo_control_all(SERVO_MED);
            }
            //ke kanan
            else if(averageX <= 6){
                tft_prints(0,3,"Turn right");
                servo_control_all(2150);
            }
            else if(averageX >= 9){
                tft_prints(0,3,"Turn left");
                servo_control_all(950);
            }
            tft_update();
            averageX = count = 0;
        
        
        
	}
    return 0;
}



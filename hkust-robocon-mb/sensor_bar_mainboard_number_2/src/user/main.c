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

int begin = -1;
int end = 0;
int length = 0;
int lastMovement = 0;
int lastTurn = 0;
float factor = 0;

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
			 //if(ticks_ms_img%50 == 0){
			 for (int i = 0; i < 16; i++) {
				 uint32_t el = sensor_output[i];
				 if (el == 1) {
					 if (begin == -1) {
						 begin = i;
					 } else {
						 end = i;
						 length++;
					 }
				 }
			 }
			 
			 // if last turn was less than 1500ms ago, go in last direction.
			 if (get_full_ticks() - lastTurn >= 300) {
				 if (length >= 3 && length <= 6) {
					 factor = (((begin+1) + (end+1)) / 2) / (float) 16;
					 lastMovement = (SERVO_MAX-200) - (factor * (SERVO_MAX - SERVO_MIN-400));
					 
					 tft_prints(0, 5, "Fek: %.4f", factor); 
				 } else if (length >= 8 || (begin == end == 1)) { // 90 degree turnnnzzz
					 if (((begin+1) + (end+1)) / 2 < 8) {
						 lastMovement = SERVO_MAX;
					 } else {
						 lastMovement = SERVO_MIN;
					 }
					 lastTurn = get_full_ticks();
				 }
					 /*else if(begin == end == 0) {
						 if(factor*16 <= 8){
							 lastMovement = SERVO_MAX;
						 }
						 else{
							 lastMovement = SERVO_MIN;
						 }
						 lastTurn = get_full_ticks();
					 }*/
				 else {
					 //fuck it, do last direction
				 }
			 }
			 
			 tft_update();
			 
			 servo_control_all(lastMovement);
			 
			 begin = -1;
			 length = 0;
	}
    return 0;
}



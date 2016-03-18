#include "sensor_bar.h"

u16 sensorbar_value[16] = {0};

void receive_a(CanRxMsg msg){
	for(int i = 0; i < 8 ;i++){
		sensorbar_value[i] = msg.Data[i];
	}
}

void receive_b(CanRxMsg msg){
	for(int i = 0; i < 8 ; i++){
		sensorbar_value[8+i] = msg.Data[i]; 
	}
}

void sensorbar_init(){
	can_init();
  can_rx_init();
  can_rx_add_filter(SENSOR_BAR_FILTER_1, CAN_RX_MASK_EXACT, receive_a);
  can_rx_add_filter(SENSOR_BAR_FILTER_2, CAN_RX_MASK_EXACT, receive_b);
}

void sensor_bar_track(){
	s8 best_start_index = 0;
	s8 best_end_index = 0;
	s8 max_width = 0;
	s8 start_index = 0;
	s8 end_index = 0;
	bool in_line = false;
	
	for (u8 index=0; index<16; index++){
		if (sensorbar_value[index] == 1){
			if (in_line){
				end_index = index;
			}else{
				start_index = end_index = index;
				in_line = true;
			}
		}else if(in_line){
			//Can skip a 0 digit
			if (index!=15 && sensorbar_value[index+1]!=1){
				in_line = false;
				if (end_index-start_index >= max_width){
					max_width = end_index-start_index;
					best_start_index = start_index;
					best_end_index = end_index;
				}
			}else{
				in_line = false;
				if (end_index-start_index >= max_width){
					max_width = end_index-start_index;
					best_start_index = start_index;
					best_end_index = end_index;
				}
			}
			if (in_line){
				end_index = 15;
				if (end_index-start_index >= max_width){
					max_width = end_index-start_index;
					best_start_index = start_index;
					best_end_index = end_index;
				}
			}
		}
	}
	s8 line_mid = (best_start_index + best_end_index) / 2;
	if (line_mid!=0){
		force_set_angle(SERVO_MED_DEG + ((int)SENSOR_BAR_MID - line_mid)*SENSOR_BAR_KP);
	}
	//force_set_angle(SERVO_MED_DEG + (line_mid - SENSOR_BAR_MID)*SENSOR_BAR_KP);
	tft_println("SE: %d %d %d %d", best_start_index, best_end_index, line_mid, ((int)SENSOR_BAR_MID - line_mid)*SENSOR_BAR_KP);
}

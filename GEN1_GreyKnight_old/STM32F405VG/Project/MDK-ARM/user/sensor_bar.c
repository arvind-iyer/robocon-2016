#include "sensor_bar.h"

u16 sensorbar_value[16] = {0};
u8 last_mid = SENSOR_BAR_MID;
float servo_angle = SERVO_MED_DEG;

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

s16 servo_pid(u8 line_mid){
	//s8 sign = (int)SENSOR_BAR_MID - line_mid < 0 ? -1 : 1;
	//s16 correction = sign*((int)SENSOR_BAR_MID - line_mid)*((int)SENSOR_BAR_MID - line_mid) * SENSOR_BAR_KP;
	s16 correction = (((int)SENSOR_BAR_MID - line_mid)*SENSOR_BAR_KP + (line_mid - last_mid) * SENSOR_BAR_KD);
	last_mid = line_mid;
	return correction;
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
			if (index==15 || sensorbar_value[index+1]!=1){
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
		}
	}
	if (in_line){
	end_index = 15;
		if (end_index - start_index >= max_width){
			max_width = end_index-start_index;
			best_start_index = start_index;
			best_end_index = end_index;
		}
	}
	s8 line_mid = (best_start_index + best_end_index) / 2;
	if (line_mid!=0){
		servo_angle += (servo_pid(line_mid)/100.0f); //Unscale by 100
		servo_angle = (servo_angle>180.0f)?180.0f:(servo_angle<0.0f?0.0f:servo_angle);
		force_set_angle(servo_angle);
	}else{
		last_mid = SENSOR_BAR_MID;
	}
	for (u8 i=0; i<16; i++){
		tft_prints(i, 5, "%d", sensorbar_value[i]);
	}
	tft_println("SE: %d %d %d %.2f", best_start_index, best_end_index, line_mid, servo_angle);
}

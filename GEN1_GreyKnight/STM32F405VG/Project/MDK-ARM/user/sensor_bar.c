#include "sensor_bar.h"

u16 sensorbar_value[16] = {0};
static u8 last_mid = SENSOR_BAR_MID;

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

SENSOR_BAR_FLAG sensor_bar_get_corr(u8 power, u16 sensor_bar_Kp, s16* corr){
	s8 best_start_index = 0;
	s8 best_end_index = 0;
	s8 max_width = 0;
	s8 start_index = 0;
	s8 end_index = 0;
	bool in_line = false;
	SENSOR_BAR_FLAG flag = SENSOR_BAR_NORM;
	
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
	
	s16 corr_angle = 0;
	if (best_start_index == 0 || best_end_index == 15){
		flag = SENSOR_BAR_EXT;
	}
	if (line_mid!=0){
		//Square the difference while maintaining sign
		s8 sign_of_error = (line_mid-SENSOR_BAR_MID)>0?1:-1;
		u16 abs_error = (line_mid-SENSOR_BAR_MID)*sign_of_error;
		u16 powered_error = 1;
		for (u8 i=0;i<power;i++){
			powered_error *= abs_error;
		}
		corr_angle = sensor_bar_Kp*powered_error*sign_of_error/100; //Unscale it
		corr_angle = (corr_angle>180)?180:(corr_angle<-180?-180:corr_angle);
	}else{
		last_mid = SENSOR_BAR_MID;
		flag = SENSOR_BAR_NTH;
	}
	tft_println("SE: %d %d %d %d", best_start_index, best_end_index, line_mid, corr_angle);
	*corr = corr_angle;
	return flag;
}

SENSOR_BAR_FLAG sensor_bar_track(u8 power, u16 sensor_bar_Kp){
	s16 correction = 0;
	SENSOR_BAR_FLAG flag = sensor_bar_get_corr(power, sensor_bar_Kp, &correction);
	if (flag != SENSOR_BAR_NTH){
		force_set_angle(SERVO_MED_DEG + correction);
	}
	return flag;
}

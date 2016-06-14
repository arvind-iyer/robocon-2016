#include "sensor_bar.h"

u16 sensor_bar_filtered[16] = {0};
u16 sensor_bar_raw[16] = {0};
u8 sensor_bar_mid = SENSOR_BAR_MID;
u8 sensorbar_region = 0;
u8 sensorbar_cali = 0;
COLOR_REGIONS region_buffer[REGION_BUFFER_SIZE] = {DOWN_GREEN};
u8 region_buffer_pointer = 0;

s16 compensated_region_color[REGIONS][3];
s16 region_color_average[REGIONS][3];
s16 raw[11] = {0};

//Receive the first half of the receiver sensor data
static void sensor_bar_receiver_a(CanRxMsg msg){
	for(int i = 0; i < 8 ;i++){
		sensor_bar_filtered[i] = sensor_bar_raw[i] & msg.Data[i];
		//sensor_bar_filtered[i] = msg.Data[i];
		sensor_bar_raw[i] = msg.Data[i];
	}
}

//Receive the second half of the receiver sensor data
static void sensor_bar_receiver_b(CanRxMsg msg){
	for(int i = 0; i < 8 ; i++){
		sensor_bar_filtered[8+i] = sensor_bar_raw[8+i] & msg.Data[i];
		//sensor_bar_filtered[8+i] = msg.Data[i];
		sensor_bar_raw[8+i] = msg.Data[i];
	}
}

//Receive the background region
static void sensor_bar_receiver_c(CanRxMsg msg){
	region_buffer[region_buffer_pointer] = msg.Data[0];
	region_buffer_pointer = (region_buffer_pointer+1)%REGION_BUFFER_SIZE;
	COLOR_REGIONS this_region = (COLOR_REGIONS) region_buffer[0];
	for (u8 i=1;i<REGION_BUFFER_SIZE;i++){
		if (this_region != region_buffer[i]){
			return;
		}
	}
	sensorbar_region = this_region;
}
 
//Receive the calibration message
static void sensor_bar_receiver_d(CanRxMsg msg){
	sensorbar_cali = msg.Data[0];
}

void sensorbar_init(){
	can_init();
  can_rx_init();
  can_rx_add_filter(SENSOR_BAR_FILTER_1, CAN_RX_MASK_EXACT, sensor_bar_receiver_a);
  can_rx_add_filter(SENSOR_BAR_FILTER_2, CAN_RX_MASK_EXACT, sensor_bar_receiver_b);
	can_rx_add_filter(SENSOR_BAR_FILTER_3, CAN_RX_MASK_EXACT, sensor_bar_receiver_c);
	can_rx_add_filter(SENSOR_BAR_FILTER_4, CAN_RX_MASK_EXACT, sensor_bar_receiver_d);
}

u16 sb_pwm_1to1(u16 inc_pwm, u16 dec_pwm, s8 sensor_bar_bias){
	return SERVO_MED_PWM - dec_pwm + (inc_pwm+dec_pwm)*(sensor_bar_mid+sensor_bar_bias)/16;
}

//Get correction without sign
s16 sensor_bar_get_corr_nf(u8 power, u16 sensor_bar_Kp){
	s8 best_start_index = 0;
	s8 best_end_index = 0;
	s8 max_width = 0;
	s8 start_index = 0;
	s8 end_index = 0;
	bool in_line = false;
	
	for (u8 index=0; index<16; index++){
		if (sensor_bar_filtered[index] == 1){
			if (in_line){
				end_index = index;
			}else{
				start_index = end_index = index;
				in_line = true;
			}
		}else if(in_line){
			//Can skip a 0 digit
			if (index==15 || sensor_bar_filtered[index+1]!=1){
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
	if (line_mid!=0){
		sensor_bar_mid = line_mid;
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
		sensor_bar_mid = SENSOR_BAR_MID;
	}
	//tft_println("SE: %d %d %d %d", best_start_index, best_end_index, line_mid, corr_angle);
	return corr_angle;
}

/**Get correction from sensor bar with flag
	SENSOR_BAR_NORM = 0, //Normal
	SENSOR_BAR_NTH = 1, //Nothing is sensed
	SENSOR_BAR_EXT = 2 //Extreme condition
	SENSOR_BAR_ALL = 3 //Read all or mostly white
*/
s16 sensor_bar_get_corr(u8 power, u16 sensor_bar_Kp, SENSOR_BAR_FLAG* in_flag){
	s8 best_start_index = 0;
	s8 best_end_index = 0;
	s8 max_width = 0;
	s8 start_index = 0;
	s8 end_index = 0;
	bool in_line = false;
	SENSOR_BAR_FLAG flag = SENSOR_BAR_NORM;
	
	for (u8 index=0; index<16; index++){
		if (sensor_bar_filtered[index] == 1){
			if (in_line){
				end_index = index;
			}else{
				start_index = end_index = index;
				in_line = true;
			}
		}else if(in_line){
			//Can skip a 0 digit
			if (index==15 || sensor_bar_filtered[index+1]!=1){
				in_line = false;
				if (end_index-start_index >= max_width){
					max_width = end_index-start_index;
					best_start_index = start_index;
					best_end_index = end_index;
				}
			}else{
				//End of 1's sequence
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
		//If the line continues to the end of sensor array
		end_index = 15;
		if (end_index - start_index >= max_width){
			//If we find a longer line
			max_width = end_index-start_index;
			best_start_index = start_index;
			best_end_index = end_index;
		}
	}
	s8 line_mid = (best_start_index + best_end_index) / 2;
	
	s16 corr_angle = 0;
	if (max_width>=ALL_WHITE_LENGTH){
		//Detect whether the line is all white
		flag = SENSOR_BAR_ALL;
	}else if (best_start_index == 0 || best_end_index == 15){
		//When the line continues to either one of the ends of the sensor array
		flag = SENSOR_BAR_EXT;
	}
	
	if (line_mid!=0){
		sensor_bar_mid = line_mid;
		//Square the difference while maintaining sign
		s8 sign_of_error = (line_mid-SENSOR_BAR_MID)>0?1:-1;
		u16 abs_error = ((s16)line_mid-(s16)SENSOR_BAR_MID)*sign_of_error;
		u16 powered_error = 1;
		for (u8 i=0;i<power;i++){
			powered_error *= abs_error;
		}
		corr_angle = sensor_bar_Kp*powered_error*sign_of_error / 100; //Unscale it
		corr_angle = (corr_angle>180)?180:(corr_angle<-180?-180:corr_angle);
	}else{
		sensor_bar_mid = SENSOR_BAR_MID;
		flag = SENSOR_BAR_NTH;
	}
	
	//tft_println("SE: %d %d %d %d", best_start_index, best_end_index, line_mid, corr_angle);
	*in_flag = flag;
	return corr_angle;
}

/*Auto tracking and adding bias using sensor bar.
	Return Flag.
	SENSOR_BAR_NORM = 0, //Normal
	SENSOR_BAR_NTH = 1, //Nothing is sensed
	SENSOR_BAR_EXT = 2 //Extreme condition
	SENSOR_BAR_ALL = 3 //Read all or mostly white
*/
SENSOR_BAR_FLAG sensor_bar_track(u8 power, u16 sensor_bar_Kp){
	SENSOR_BAR_FLAG flag;
	s16 correction = sensor_bar_get_corr(power, sensor_bar_Kp, &flag);
	if (flag != SENSOR_BAR_NTH){
		si_add_pwm_bias(correction*10);
	}
	return flag;
}

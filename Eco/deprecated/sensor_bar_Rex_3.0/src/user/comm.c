#include "comm.h"

static bool sending_out_color = false;
static bool receiving_color = false;
static bool color_received[REGIONS][2] = {false};

//Receive the color request command
static void color_request_receiver(CanRxMsg msg){
	sending_out_color = true;
}

static void color_replacement_receiver(CanRxMsg msg){
	receiving_color = true;
	u8 region_id = msg.Data[0];
	bool is_background = msg.Data[1]; // 0 is mid, 1 is background
	color_received[region_id][is_background] = true;
	s16 rgb[3] = {0};
	
	for (u8 i=0;i<3;i++){
		rgb[i] = (msg.Data[2*i+2] << 8) & (msg.Data[2*i+3]);
		if (is_background){
			compensated_region_color[region_id][i] = rgb[i];
		}else{
			region_color_average[region_id][i] = rgb[i];
		}
	}
}

void comm_init(){
	can_rx_add_filter(SENSOR_BAR_REQUEST_COLOR, CAN_RX_MASK_EXACT, color_request_receiver);
	can_rx_add_filter(SENSOR_BAR_REPLACE_COLOR, CAN_RX_MASK_EXACT, color_replacement_receiver);
}

void comm_update(){
	if (sending_out_color){
		for (u8 region=0;region<REGIONS;region++){
				CAN_MESSAGE color_msg;
				color_msg.id = SENSOR_BAR_AVG_COLOR_RETURN;
				color_msg.length = 7;
				color_msg.data[0] = region;
				for (u8 i=0;i<6;i++){
					color_msg.data[i+1] = one_to_n_bytes(region_color_average[region][i/2], i%2);
				}
				
				CAN_MESSAGE white_msg;
				white_msg.id = SENSOR_BAR_WHITE_COLOR_RETURN;
				white_msg.length = 7;
				white_msg.data[0] = region;
				for (u8 i=0;i<6;i++){
					white_msg.data[i+1] = one_to_n_bytes(compensated_region_color[region][i/2], i%2);
				}
				
				can_tx_enqueue(color_msg);
				can_tx_enqueue(white_msg);
				_delay_ms(100);
		}
		sending_out_color = false;
	}
	
	if (receiving_color){
		u8 received_count = 0;
		for (u8 i=0;i<REGIONS;i++){
			for (u8 k=0;k<2;k++){
				received_count += color_received[i][k];
			}
		}
		CAN_MESSAGE received_count_msg;
		received_count_msg.id = SENSOR_BAR_REPLACED_CNT;
		received_count_msg.length = 1;
		received_count_msg.data[0] = receiving_color;
		can_tx_enqueue(received_count_msg);
		if (received_count == REGIONS*2){
			receiving_color = false;
			writeFlash();
			_delay_ms(200);
		}
	}
}

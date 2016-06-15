#include "comm.h"

static bool sending_out_color = false;

//Receive the color request command
static void color_request_receiver(CanRxMsg msg){
	sending_out_color = true;
}

void comm_init(){
	can_rx_add_filter(SENSOR_BAR_REQUEST_COLOR, CAN_RX_MASK_EXACT, color_request_receiver);
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
}

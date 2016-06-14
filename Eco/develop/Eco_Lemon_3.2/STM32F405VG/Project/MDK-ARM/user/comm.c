#include "comm.h"

//Receive the requested color
static void sensor_bar_cali_color_receiver(CanRxMsg msg){
	u8 flag = 0;
	s16 data[3] = {0};
	for (u8 i=0;i<3;i++){
		data[i] = (msg.Data[i*2+1] | msg.Data[i*2+2]<<8);
	}
	
	if (msg.StdId == SENSOR_BAR_AVG_COLOR_RETURN){
		flag |= 128; //1000 0000
		for (u8 i=0;i<3;i++){
			region_color_average[msg.Data[0]][i] = data[i];
		}
	}else{
		flag |= 0;
		for (u8 i=0;i<3;i++){
			compensated_region_color[msg.Data[0]][i] = data[i];
		}
				if (msg.Data[0] == 0){
			for (u8 i=0;i<8;i++){
				raw[i] = msg.Data[i];
			}
			raw[8] = data[0];
			raw[9] = data[1];
			raw[10] = data[2];
		}
	}
	
	flag |= msg.Data[0]; //0~18
	
	u8 r_flag = flag|32; //red 
	u8 g_flag = flag|64; //green 
	u8 b_flag = flag|96; //blue 
	uart_tx_byte(SENSOR_BAR_UART_COM, r_flag);
	uart_tx_byte(SENSOR_BAR_UART_COM, data[0]>>8);
	uart_tx_byte(SENSOR_BAR_UART_COM, data[0]&255);
	
	uart_tx_byte(SENSOR_BAR_UART_COM, g_flag);
	uart_tx_byte(SENSOR_BAR_UART_COM, data[1]>>8);
	uart_tx_byte(SENSOR_BAR_UART_COM, data[1]&255);
	
	uart_tx_byte(SENSOR_BAR_UART_COM, b_flag);
	uart_tx_byte(SENSOR_BAR_UART_COM, data[2]>>8);
	uart_tx_byte(SENSOR_BAR_UART_COM, data[2]&255);
}

void comm_init(){
	uart_init(SENSOR_BAR_UART_COM, 115200);
	can_rx_add_filter(SENSOR_BAR_AVG_COLOR_RETURN, CAN_RX_MASK_EXACT, sensor_bar_cali_color_receiver);
	can_rx_add_filter(SENSOR_BAR_WHITE_COLOR_RETURN, CAN_RX_MASK_EXACT, sensor_bar_cali_color_receiver);
}

void request_color_msg(){
	CAN_MESSAGE request;
	request.id = SENSOR_BAR_REQUEST_COLOR;
	request.length = 0;
	can_tx_enqueue(request);
}

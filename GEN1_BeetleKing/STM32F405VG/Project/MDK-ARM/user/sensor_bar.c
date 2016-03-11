#include "sensor_bar.h"

//u16 sensorbar_value[16] = {0};

//void receive_a(CanRxMsg msg){
//	for(int i = 0; i < 8 ;i++){
//		sensorbar_value[i] = msg.Data[i];
//	}
//}

//void receive_b(CanRxMsg msg){
//	for(int i = 0; i < 8 ; i++){
//		sensorbar_value[8+i] = msg.Data[i]; 
//	}
//}

//void sensorbar_init(void){
//	can_init();
//  can_rx_init();
//  can_rx_add_filter(SENSOR_BAR_FILTER_1, CAN_RX_MASK_EXACT, receive_a);
//  can_rx_add_filter(SENSOR_BAR_FILTER_2, CAN_RX_MASK_EXACT, receive_b);
//}

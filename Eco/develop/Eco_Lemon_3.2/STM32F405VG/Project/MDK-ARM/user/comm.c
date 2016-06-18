#include "comm.h"

static void replace_color(u8 data[8]);
static void comm_uart_receiver(u8 data);
static void sensor_bar_cali_color_receiver(CanRxMsg msg);
static void sensor_bar_replaced_receiver(CanRxMsg msg);

u16 received_color_count = 0;

void comm_init(){
	//UART for communicating with PC
	uart_init(SENSOR_BAR_UART_COM, 115200);
	uart_interrupt_init(SENSOR_BAR_UART_COM, comm_uart_receiver);
	
	//CAN for communicating with sensor bar
	can_rx_add_filter(SENSOR_BAR_AVG_COLOR_RETURN, CAN_RX_MASK_EXACT, sensor_bar_cali_color_receiver);
	can_rx_add_filter(SENSOR_BAR_WHITE_COLOR_RETURN, CAN_RX_MASK_EXACT, sensor_bar_cali_color_receiver);
	can_rx_add_filter(SENSOR_BAR_REPLACED_CNT, CAN_RX_MASK_EXACT, sensor_bar_replaced_receiver);
}

//Receive the requested color
static void sensor_bar_cali_color_receiver(CanRxMsg msg){
	u8 flag = 0;
	s16 data[3] = {0};
	for (u8 i=0;i<3;i++){
		data[i] = (msg.Data[i*2+1] | msg.Data[i*2+2]<<8);
	}
	
	if (msg.StdId == SENSOR_BAR_AVG_COLOR_RETURN){
		flag |= 128; //0b 1000 0000
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
	uart_tx_byte(SENSOR_BAR_UART_COM, data[0] >> 8);
	uart_tx_byte(SENSOR_BAR_UART_COM, data[0] & 255);
	
	uart_tx_byte(SENSOR_BAR_UART_COM, g_flag);
	uart_tx_byte(SENSOR_BAR_UART_COM, data[1] >> 8);
	uart_tx_byte(SENSOR_BAR_UART_COM, data[1] & 255);
	
	uart_tx_byte(SENSOR_BAR_UART_COM, b_flag);
	uart_tx_byte(SENSOR_BAR_UART_COM, data[2] >> 8);
	uart_tx_byte(SENSOR_BAR_UART_COM, data[2] & 255);
}

static void sensor_bar_replaced_receiver(CanRxMsg msg){
	received_color_count = msg.Data[0];
}

static UART_COMM_STATE uart_state = NOT_RECEIVING_ANYTHING;
static u8* uart_data_buffer; //Dynamically allocate memory for this array, based on the required size
static u16 uart_data_pointer = 0;

//Free the memory, reset pointer and uart state
static void comm_uart_free(){
	uart_data_pointer = 0;
	free(uart_data_buffer);
	uart_state = NOT_RECEIVING_ANYTHING;
}

static void comm_uart_receiver(u8 data){
	switch (uart_state){
		
		case NOT_RECEIVING_ANYTHING:
			switch (data){
				case UART_REPLACE_COLOR_START_FLAG:
					uart_state = RECEIVING_REPLACEMENT_COLOR;
					uart_data_buffer = calloc(UART_REPLACE_COLOR_LENGTH, sizeof(u8));
				break;
			}
		break;
			
		case RECEIVING_REPLACEMENT_COLOR:
			if (uart_data_pointer < UART_REPLACE_COLOR_LENGTH){
				uart_data_buffer[uart_data_pointer++] = data;
			}else{
				replace_color(uart_data_buffer);
				comm_uart_free();
			}
		break;
	}
}

void request_color_msg(){
	CAN_MESSAGE request;
	request.id = SENSOR_BAR_REQUEST_COLOR;
	request.length = 0;
	can_tx_enqueue(request);
}

//A direct copy approach
void replace_color(u8 data[8]){
	CAN_MESSAGE replacement;
	replacement.id = SENSOR_BAR_REPLACE_COLOR;
	replacement.length = 8;
	memcpy(replacement.data, data, 8*sizeof(u8));
	can_tx_enqueue(replacement);
}

//How it is actually done
//void replace_color(u8 region, bool is_background, s16 rgb[3]){
//	CAN_MESSAGE replacement;
//	replacement.id = SENSOR_BAR_REPLACE_COLOR;
//	replacement.length = 8;
//	replacement.data[0] = region;
//	replacement.data[1] = is_background;
//	for (u8 i=0;i<3;i++){
//		replacement.data[i*2+2] = rgb[i] >> 8;
//		replacement.data[i*2+3] = rgb[i] & 255; //mask of 0b 11111111
//	}
//	can_tx_enqueue(replacement);
//}

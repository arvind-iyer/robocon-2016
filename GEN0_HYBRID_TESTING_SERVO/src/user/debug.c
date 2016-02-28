#include "debug.h"

static u8 dataset_id = 0;

void debug_init(){
	bluetooth_init();
}

void new_graph(u8 graph_id, char* graph_name, char* xAxis, char* yAxis){
	bluetooth_tx_byte(RECEIVE_PACKET_START);
	bluetooth_tx_byte(INSTRUCTION_SET_NEW_VAR);
	bluetooth_tx_byte(dataset_id++);
	bluetooth_tx_byte(graph_id);
	
	while(*graph_name!='!'){
		bluetooth_tx_byte(*graph_name);
		graph_name++;
	}
	bluetooth_tx_byte('!');
	
	while(*xAxis!='!'){
		bluetooth_tx_byte(*xAxis);
		xAxis++;
	}
	bluetooth_tx_byte('!');
	
	while(*yAxis!='!'){
		bluetooth_tx_byte(*yAxis);
		yAxis++;
	}
	bluetooth_tx_byte('!');
	
}

void send_data(){
}
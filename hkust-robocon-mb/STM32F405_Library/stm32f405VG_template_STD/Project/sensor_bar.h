#ifndef __SENSORBARH
#define __SENSORBARH
#include "can_protocol.h"
#include "lcd.h"

#define ADDRESS1 0x0C5 //Its not a variable!
#define ADDRESS2 0x0C6 //It will be replaced during compilation !!!

//Internal functions
void receive(CanRxMsg msg);
void receive(CanRxMsg msg2);


void sensorbar_init(void);
void get_sensor_data(void);
void fill_array(void);
#endif
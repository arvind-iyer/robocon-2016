#ifndef _SENSORBAR_H
#define _SENSORBAR_H

#include "can_protocol.h"

#define SENSOR_BAR_FILTER_1 0x0C5
#define SENSOR_BAR_FILTER_2 0x0C6 

void sensorbar_init(void);
void sensor_bar_update(void);

extern u16 sensorbar_value[16];

#endif

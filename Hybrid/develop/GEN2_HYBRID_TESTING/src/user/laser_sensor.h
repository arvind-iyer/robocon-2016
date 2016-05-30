#ifndef __LASER_SENSOR_H
#define __LASER_SENSOR_H

#include "adc.h"
#define ls_number					3				//number of sensors used

u32 get_ls_cal_reading(u8 device);
u32 get_ls_adc_reading(u8 device);

#endif	

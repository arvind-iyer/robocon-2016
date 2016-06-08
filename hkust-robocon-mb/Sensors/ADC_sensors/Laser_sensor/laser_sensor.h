#ifndef __LASER_SENSOR_H
#define __LASER_SENSOR_H

#include "adc.h"
#define ls_number		3				//number of sensors used
#define Kq					10		
#define	Kr					2
#define Kscale			1000
#define LPcons			0.725

u32 get_ls_cal_reading(u8 device);
u32 get_ls_adc_reading(u8 device);
u32 lowpass_filter(u32 input);

#endif	

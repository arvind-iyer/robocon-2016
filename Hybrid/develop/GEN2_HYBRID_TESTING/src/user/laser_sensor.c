/**
*	Laser Sensor for stm32f103
*	@Desciption This is the library for the SICK DT50 laser sensor, which mesaures the distance from 200mm to 5000mm with 3.5% error from the board
*	@Reminder 	A 220nF capacitor must be presented at the adc port in order to filter out the noise, default ADC used are ADC_1, ADC_2 and ADC_3
*	@Author 		James Mok
**/

#include "laser_sensor.h"

static u16 ls_adc_reading[ls_number];
static u16 ls_cal_reading[ls_number];
static u16 ls_dma_reading[ls_number];

static const u16 min_adc[ls_number] = {5, 5, 16};
static const u16 max_adc[ls_number] = {2910, 2910, 2910};
static const u16 min_dis[ls_number] = {200, 200, 200};
static const u16 max_dis[ls_number] = {2500, 2500, 2500};

/**
*	@brief return the range found
*	@Param	The index of the ADC data array
* @return the calculated range
**/

u32 get_ls_cal_reading(u8 device)
{
	if(get_adc(device)>=min_adc[device])
	{
		ls_cal_reading[device] = (get_adc(device) - min_adc[device]) * (max_dis[device] - min_dis[device]) / (max_adc[device] - min_adc[device]) + min_dis[device];
		if(ls_cal_reading[device] >= max_dis[device])
			ls_cal_reading[device] = max_dis[device];
	}
	else
		ls_cal_reading[device] = 0;
	
	return ls_cal_reading[device];
}

/**
*	@brief return the ADC value for tuning the static const array above
*	@Param	The index of the ADC data array
* @return the ADC value of the sensors
**/

u32 get_ls_adc_reading(u8 device)
{
	ls_adc_reading[device] = get_adc(device);
	return ls_adc_reading[device];
}
#ifndef	__colour_calculation
#define	__colour_calculation

#include "stm32f10x.h"
#include "led.h"
#include "can_protocol.h"
#include "approx_math.h"
#include "delay.h"
#include "stm32f10x_adc.h"

#define max_turn 20	//40 
#define max_reading 2700 
#define list_num 2 //the max number that the device remember
#define white_tolerance 10
#define rgb_tolerance_r 80
#define rgb_tolerance_g 50 //the tolerance for differentiate colours, decrease will provide a accuracy reading for one colour, vice verse
#define rgb_tolerance_b 50
#define sample_time 20 //20
#define delay_time 1500
#define ratio 1000
#define scale 2000

struct Reading
{
	u16 red_reading[16];
	u16 blue_reading[16];
	u16 green_reading[16];
	u16 off_reading[16];
	u16 h[16];
	u16 s[16];
	u16 v[16];
};

#define RED 0
#define GREEN 1
#define BLUE 2
#define OFF 3

void writeFlash(void);
void readFlash(void);	

void color_array_init();
void sensor_init();
//void color_max_init();
void data_collect(u8, u8);
void avg_reading();
void colour_init();

u8 colour_compare(struct Reading* reading, u8 index);

void set_colour_list(u8);

void read_reading(struct Reading* reading, u8 colour, u8 position, u16 id);
void read_single_reading(struct Reading* reading, u8 pos);
void read_colour();

void rgb_to_hsv_degree(struct Reading* reading);
void rgb_check_reading(struct Reading* reading);
void rgb_colour_difference(struct Reading* reading);

#endif


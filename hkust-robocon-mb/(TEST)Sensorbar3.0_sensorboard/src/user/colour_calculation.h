#include "stm32f10x.h"
#include "colour_list.h"
#include "led.h"
#include "can_protocol.h"
#include "approx_math.h"
#include "delay.h"
#include "stm32f10x_adc.h"
#include "math.h"

#define max_turn 90
#define max_reading 2700

#define RED 0
#define BLUE 1
#define GREEN 2
#define OFF 3


#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y)) 

/**
	* @breif stuct for storing the readings for red, blue, green
**/
struct Reading
{
	u32 red_reading[16];
	u32 blue_reading[16];
	u32 green_reading[16];
};

void data_collect();
void colour_init();
u8 colour_compare(struct Reading* reading, u8 index);

void inc_temp_readings(struct Reading* reading, u8 colour);
void avg_temp_readings(struct Reading* reading, u8 freq);
void set_colour_list(u8 index);

void temp_reading_init(struct Reading* reading);
void read_reading(struct Reading* reading, u8 colour, u8 position);
void read_single_reading(struct Reading* reading, u8 pos);

void rgb_to_hsv(struct Reading* reading);
void rgb_to_hsv_degree(struct Reading* reading);
u32 function_t(u32 temp);
u32 colour_difference(struct Reading* reading, struct Colour* list, u8 index);
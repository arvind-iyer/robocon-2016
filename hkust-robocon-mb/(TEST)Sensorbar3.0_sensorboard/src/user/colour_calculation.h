#include "stm32f10x.h"
#include "colour_list.h"
#include "led.h"
#include "can_protocol.h"
#include "approx_math.h"
#include "delay.h"
#include "stm32f10x_adc.h"

#define max_turn 20	//40 
#define max_reading 2700 
#define list_num 10 
#define white_tolerance 10 
#define sample_time 20 //20
#define delay_time 5 

void data_collect();
void colour_init();
void rgb_init();
void reading_ratio();
void rgb_normalization(struct Reading* reading);
u8 colour_compare(struct Reading* reading, u8 index);

void temp_reading_init(struct Reading* reading);
void read_reading(struct Reading* reading, u8 colour, u8 position);
void read_single_reading(struct Reading* reading, u8 pos);

void rgb_to_hsv_degree(struct Reading* reading);
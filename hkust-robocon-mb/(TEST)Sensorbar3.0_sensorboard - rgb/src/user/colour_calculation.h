#include "stm32f10x.h"
#include "colour_list.h"
#include "led.h"
#include "can_protocol.h"
#include "approx_math.h"
#include "delay.h"
#include "stm32f10x_adc.h"

#define max_turn 40	//40 
#define max_reading 2700 
#define list_num 4 //the max number that the device remember
#define white_tolerance 10
#define rgb_tolerance_r 100
#define rgb_tolerance_g 80 //the tolerance for differentiate colours, decrease will provide a accuracy reading for one colour, vice verse
#define rgb_tolerance_b 80
#define sample_time 20 //20
#define delay_time 1500
#define ratio 1000

void writeFlash(void);
void readFlash(void);	

void data_collect();
void colour_init();
void rgb_init();
void reading_ratio();
void rgb_normalization(struct Reading* reading);
u8 colour_compare(struct Reading* reading, u8 index);

void temp_reading_init(struct Reading* reading);
void read_reading(struct Reading* reading, u8 colour, u8 position);
void read_single_reading(struct Reading* reading, u8 pos);
void read_colour();

void rgb_to_hsv_degree(struct Reading* reading);
void rgb_check_reading(struct Reading* reading);
void rgb_colour_difference(struct Reading* reading);
#include <stdlib.h>
#include "stm32f10x.h"

struct Reading
{
	u16 red_reading[16];
	u16 blue_reading[16];
	u16 green_reading[16];
};

#define RED 0
#define GREEN 1
#define BLUE 2
#define OFF 3

void set_temp_readings(struct Reading* reading, u8 turn, u8 colour);
void copy_readings(struct Reading* reading, struct Reading* target, u8 index);
void avg_temp_readings(struct Reading* reading, struct Reading* target, u8 max_turn);
void set_colour_list(u8 index);
void reading_init(struct Reading* reading);
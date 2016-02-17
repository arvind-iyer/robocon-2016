#include <stdlib.h>
#include "stm32f10x.h"

struct Colour
{
	u8 id;
	u16 red_reading[16];
	u16 blue_reading[16];
	u16 green_reading[16];
};

#define RED_ID 0
#define BLUE_ID 1
#define GREEN_ID 2
#define WHITE_ID 3

void set_id(struct Colour*, u8);
void set_red(struct Colour*, u32*);
void set_blue(struct Colour*, u32*);
void set_green(struct Colour*, u32*);
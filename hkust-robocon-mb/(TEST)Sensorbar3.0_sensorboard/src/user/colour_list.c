#include "colour_list.h"

void set_id(struct Colour* list_ptr, u8 id)
{
	list_ptr->id = id;
}

void set_red(struct Colour* list_ptr, u32* array_ptr)
{
	for(u8 i=0;i<16;i++)
		list_ptr->red_reading[i] = *(array_ptr + i);
}

void set_blue(struct Colour* list_ptr, u32* array_ptr)
{
	for(u8 i=0;i<16;i++)
		list_ptr->blue_reading[i] = *(array_ptr + i);
}

void set_green(struct Colour* list_ptr, u32* array_ptr)
{
	for(u8 i=0;i<16;i++)
		list_ptr->green_reading[i] = *(array_ptr + i);
}


#ifndef __LINE_TRACK_H
#define __LINE_TRACK_H

#define LEFT 15
#define RIGHT 113
#define LIMIT 5
#define WIDTH 10
#define ALL_WHITE 127

#include "ticks.h"
#include "tft_display.h"
#include "linear_ccd.h"
#include "motor.h"

extern u16 white_threshold1,white_threshold2;
extern u16 black_threshold1,black_threshold2;
extern u8 turn_count;

u8 line_track_init(u8);
u8 line_track(u8);
void crappy_line_track(u8);
u8 read();
u8 line();
void halt();
void turn(u8);
void go(u16);
void arc(u16,u8);

#endif

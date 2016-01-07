#include "strike.h"

int16_t target_x, target_y;
int16_t curr_x, curr_y;
int16_t target_direction = 0;

void setTarget(int16_t x, int16_t y){
	target_x = x;
	target_y = y;
}

/***************************************
/ @attention
/ For the more circular base, 
/ X-Y invert
/ Then Y-direction +ve -ve invert
/
****************************************/

void map(){

	curr_x = get_Y();
	curr_y = -get_X();
	tft_clear();
	tft_prints(0, 0, "X:%d Y:%d", curr_x, curr_y);
	tft_prints(0, 1, "X:%d Y:%d", target_x, target_y);
	target_direction = (int_arc_tan2(target_y - curr_x, target_x - curr_y) - get_angle()/10) %360;
	target_direction = target_direction < 0 ? target_direction + 360 : target_direction;
	tft_prints(0, 2, "Bearing: %d", target_direction);
	tft_update();
	
	
	//Redraw coordinate
	for (u16 i = 45;i<160;i++){
		tft_put_pixel(64, i, WHITE);
	}

	for (u16 i = 0;i<128;i++){
		tft_put_pixel(i, 110, WHITE);
	}
	
	for (u16 i=61;i<=67;i++){
		for (u16 j=107;j<=113;j++){
			tft_put_pixel(i, j, RED);
		}
	}
}
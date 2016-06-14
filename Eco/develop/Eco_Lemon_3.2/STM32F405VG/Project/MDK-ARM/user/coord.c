#include "coord.h"

s32 unscale_x = 0, unscale_y = 0, unscale_z = 0;
static s32 last_enc_reading = 0;


void coord_update(){
	s32 this_enc = get_average_encoder();
	s32 enc_diff = this_enc - last_enc_reading;
//	unscale_x += int_cos(mti_int_ypr[0])*int_cos(mti_int_ypr[1])/10000*enc_diff/100;
//	unscale_y += int_sin(mti_int_ypr[0])*int_cos(mti_int_ypr[1])/10000*enc_diff/100;
//	unscale_z += int_sin(mti_int_ypr[1])*enc_diff/100;
	unscale_x += int_cos(mti_int_ypr[0])*int_cos(mti_int_ypr[1])/10000*enc_diff/100;
	unscale_y += int_sin(mti_int_ypr[0])*int_cos(mti_int_ypr[1])/10000*enc_diff/100;
	unscale_z += int_sin(mti_int_ypr[1])*enc_diff/100;
	last_enc_reading = this_enc;
}

s32 get_x(){
	return unscale_x/100*ENCODER_RATIO;
}

s32 get_y(){
	return unscale_y/100*ENCODER_RATIO;
}

s32 get_z(){
	return unscale_z/100*ENCODER_RATIO;
}

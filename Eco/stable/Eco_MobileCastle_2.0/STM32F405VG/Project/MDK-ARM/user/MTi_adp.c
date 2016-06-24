#include "MTi_adp.h"

float mti_start_ypr[3] = {0};
s16 mti_int_ypr[3] = {0};
s16 mti_int_acc[3] = {0};
s16 mti_start_bias_yaw = 0;

static s32 first_init_ticks = 0;
static bool ready = false;
static bool second_reset = false;

bool mti_all_ready(){
	return ready;
}

void mti_immediate_ready(){
	second_reset = true;
}

void mti_init(){
	ready = second_reset = false;
	_delay_ms(120);
	MTi_1_UART_init();
	first_init_ticks = get_full_ticks();
}

void mti_update(){
	
	mti_int_ypr[0] = (s16)roundf(get_MTi_ang(2)*10)+mti_start_bias_yaw;
	mti_int_ypr[1] = (s16)roundf(get_MTi_ang(1)*10);
	mti_int_ypr[2] = (s16)roundf(get_MTi_ang(0)*10);
	
	for (u8 i=0;i<3;i++){
		mti_int_acc[i] = (s16)roundf(get_MTi_acc(i)*10);
	}
	
	if (!second_reset && (this_loop_ticks - first_init_ticks) > 500){
		MTi_1_UART_init();
		second_reset = true;
		buzzer_play_song(SUCCESSFUL_SOUND, 100, 0);
	}
	
	if (second_reset && !ready){
		bool flag = false;
		for (u8 i=0;i<3;i++){
			if (get_MTi_ang(i) != 0.0f || get_MTi_acc(i) != 0.0f){
				flag = true;
			}
		}
		if (flag){
			ready = true;
			for (u8 i=0;i<3;i++){
				mti_start_ypr[i] = get_MTi_ang(2-i);
			}
		}
	}
}

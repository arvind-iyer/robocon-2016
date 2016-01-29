#include "main.h"

//uint16_t servo_val = SERVO_MIN;

#define acc_stepping 5
#define acc_threshold 30
s32 target_speed = 0;
s32 target_rotate = 0;
s32 curr_speed = 0;
s32 curr_angle = 0;
s32 curr_rotate = 0;

void updateSpeed(){
	if ((curr_speed*curr_speed - target_speed*target_speed)>acc_threshold){
		if (curr_speed>target_speed){
			curr_speed -= acc_stepping;
		}else{
			curr_speed += acc_stepping;
		}
	}else{
		curr_speed = target_speed;
	}
	
	if ((curr_rotate*curr_rotate - target_rotate*target_rotate)>acc_threshold){
		if (curr_rotate>target_rotate){
			curr_rotate -= acc_stepping;
		}else{
			curr_rotate += acc_stepping;
		}
	}else{
		curr_rotate = target_rotate;
	}
	
	motor_set_vel(MOTOR1, int_sin(curr_angle)*curr_speed*(-1)/10000 + curr_rotate, CLOSE_LOOP);
	motor_set_vel(MOTOR2, int_sin(curr_angle+1200)*curr_speed*(-1)/10000 + curr_rotate, CLOSE_LOOP);
	motor_set_vel(MOTOR3, int_sin(curr_angle+2400)*curr_speed*(-1)/10000 + curr_rotate, CLOSE_LOOP);
}

//Angle *10
void setGlobal(s32 angle, s32 speed, s32 rotate){
	curr_angle = angle;
	target_speed = speed;
	target_rotate = rotate;
}

int main(void) 
{
	servo_init();
	led_init();
	ticks_init();
	tft_easy_init();
	can_init();
	can_rx_init();
	can_motor_init();
	xbc_mb_init(XBC_CAN_FIRST);
	//can_xbc_mb_tx_enable(true);

	tft_put_logo(110, 90);
	u32 lastTicks = 0;
	
	while(1){
		if ((lastTicks - get_full_ticks())>50){
			lastTicks = get_full_ticks();
			led_blink(LED_D1);
			button_update();
			updateSpeed();
			
			tft_clear();
			tft_append_line("%d", xbc_get_joy(XBC_JOY_LX));
			tft_append_line("%d", xbc_get_joy(XBC_JOY_LY));
			tft_append_line("%d", xbc_get_joy(XBC_JOY_RX));
			tft_append_line("%d", xbc_get_joy(XBC_JOY_RY));
			
			tft_append_line("%d", xbc_get_connection());
			tft_append_line("%d", get_ticks());
	
			s32 angle = int_arc_tan2(xbc_get_joy(XBC_JOY_LX), xbc_get_joy(XBC_JOY_LY))*10;
			s32 speed = (xbc_get_joy(XBC_JOY_LY)*xbc_get_joy(XBC_JOY_LY) + 
										xbc_get_joy(XBC_JOY_LX)*xbc_get_joy(XBC_JOY_LX))/10000*60/100;
			s8 rotate = button_pressed(BUTTON_XBC_A)?10:(button_pressed(BUTTON_XBC_B)?-10:0);
			
			setGlobal(angle, speed, rotate);
			
			tft_update();
		}
	}
}
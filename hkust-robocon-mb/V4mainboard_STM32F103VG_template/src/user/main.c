#include "main.h"

//uint16_t servo_val = SERVO_MIN;

//Angle *10
void setGlobal(s32 angle, s32 speed, s32 rotate){
	motor_set_vel(MOTOR1, int_sin(angle)*speed*(-1)/10000 + rotate, CLOSE_LOOP);
	motor_set_vel(MOTOR2, int_sin(angle+1200)*speed*(-1)/10000 + rotate, CLOSE_LOOP);
	motor_set_vel(MOTOR3, int_sin(angle+2400)*speed*(-1)/10000 + rotate, CLOSE_LOOP);
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

	tft_put_logo(110, 90);

	while(1){
		if (get_full_ticks()%50==0){
			led_blink(LED_D1);
			button_update();
			
			tft_clear();
			tft_append_line("%d", xbc_get_joy(XBC_JOY_LX));
			tft_append_line("%d", xbc_get_joy(XBC_JOY_LY));
			tft_append_line("%d", button_pressed(BUTTON_XBC_A));
			tft_append_line("%d", button_pressed(BUTTON_XBC_B));
			
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
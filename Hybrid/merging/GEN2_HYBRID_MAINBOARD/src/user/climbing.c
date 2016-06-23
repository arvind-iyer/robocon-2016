#include "climbing.h"

static u8 climbing_stage = 0;
static s32 climbing_pneumatic_ticks = 0;
//delay->up->delay->push->delay->unclaw->delay->retract->delay->beep
static u8 putting_propeller_state = 0;
static s32 putting_propeller_ticks[5] = {0};

void climbing_init(){
	climbing_stage = 0;
}

u8 climbing_update(){
	raise_arm();
	
	brushless_power_percent = 20 + get_angle()>1800?0:get_angle()*CLIMBING_BRUSHLESS_P/1000;
	brushless_control(brushless_power_percent, true);
	
	switch (climbing_stage){
		case 0:
			pneumatic_on(&CLIMB_PNEUMATIC_PORT);
			
			brushless_servo_val = CLIMBING_BRUSHLESS_ANGLE;
			brushless_servo_control(brushless_servo_val);
			brushless_control(brushless_power_percent, true);
			
			climbing_stage++;
			climbing_pneumatic_ticks = this_loop_ticks;
			break;
		
		case 1:
			if((this_loop_ticks - climbing_pneumatic_ticks)>CLIMBING_TICKS_LIMIT){
				climb_continue();
				if (gpio_read_input(HIT_BOX_R_PORT) && gpio_read_input(HIT_BOX_L_PORT)){
					climbing_stage++;
					putting_propeller_ticks[0] = this_loop_ticks;
					putting_propeller_state = 0;
					stop_climbing();
					brushless_power_percent = 0;
					brushless_control(brushless_power_percent, true);
				}
			}
			break;
			
		case 2:
			if (putting_propeller_state == 0 && ((this_loop_ticks - putting_propeller_ticks[putting_propeller_state]) > PUTTING_PROPELLER_UP_DELAY)){
				putting_propeller_state++;
				gripper_down = GRIPPER_FULL_UP;
				gripper_control(THIS_GRIPPER, gripper_down);
				putting_propeller_ticks[1] = this_loop_ticks;
				
			}else	if (putting_propeller_state == 1 && ((this_loop_ticks - putting_propeller_ticks[putting_propeller_state]) > PUTTING_PROPELLER_PUSH_DELAY)){
				putting_propeller_state++;
				gripper_extended = false;
				gripper_push_control(THIS_GRIPPER, gripper_extended);
				putting_propeller_ticks[2] = this_loop_ticks;
				
			}else	if (putting_propeller_state == 2 && ((this_loop_ticks - putting_propeller_ticks[putting_propeller_state]) > PUTTING_PROPELLER_UNCLAW_DELAY)){
				gripper_clawed = false;
				gripper_claw_control(THIS_GRIPPER, gripper_clawed);
				putting_propeller_state++;
				putting_propeller_ticks[3] = this_loop_ticks;
				
			}else	if (putting_propeller_state == 3 && ((this_loop_ticks - putting_propeller_ticks[putting_propeller_state]) > PUTTING_PROPELLER_RETRACT_DELAY)){
				gripper_down = GRIPPER_FULL_DOWN;
				gripper_control(THIS_GRIPPER, gripper_down);
				putting_propeller_state++;
				putting_propeller_ticks[4] = this_loop_ticks;
				
			}else	if (putting_propeller_state == 4 && ((this_loop_ticks - putting_propeller_ticks[putting_propeller_state]) > PUTTING_PROPELLER_YEAH_DELAY)){
				putting_propeller_state++;
			}
			break;
	}
	
	return 3;
}


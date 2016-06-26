#include "climbing.h"

static u8 climbing_stage = 0;
static s32 climbing_pneumatic_ticks = 0;
//delay->up->delay->push->delay->unclaw->delay->retract->delay->beep
static u8 putting_propeller_state = 0;
static s32 putting_propeller_ticks[5] = {0};
static u32 climbing_start_ticks = 0;

void climbing_init(){
	climbing_stage = 0;
	climbing_start_ticks = this_loop_ticks;
}

u8 climbing_update(){
	raise_arm();

	if ((this_loop_ticks - climbing_start_ticks) < 2000){
		brushless_servo_val = 0;
		brushless_servo_control(brushless_servo_val);
		brushless_power_percent = 80;
		brushless_control(brushless_power_percent, true);
		
	}else{
		if (climbing_stage <= 1){
			brushless_servo_val = CLIMBING_BRUSHLESS_ANGLE;
			brushless_servo_control(brushless_servo_val);
			brushless_power_percent = 35 + get_angle()>1800?0:get_angle()*CLIMBING_BRUSHLESS_P/1000;
			brushless_control(brushless_power_percent, true);
		}else{
			brushless_power_percent = 0;
			brushless_control(brushless_power_percent, true);
		}
	}
	
	switch (climbing_stage){
		case 0:
			pneumatic_on(&CLIMB_PNEUMATIC_PORT);
			
			climbing_stage++;
			climbing_pneumatic_ticks = this_loop_ticks;
			break;
		
		case 1:
			if((this_loop_ticks - climbing_pneumatic_ticks)>CLIMBING_TICKS_LIMIT){
				if (this_loop_ticks - climbing_pneumatic_ticks > CLIMBING_SLOW_LIMIT){
					slow_climb_continue();
				}else{
					climb_continue();
				}
				if (gpio_read_input(HIT_BOX_R_PORT) || gpio_read_input(HIT_BOX_L_PORT)){
					climbing_stage++;
					putting_propeller_ticks[0] = this_loop_ticks;
					putting_propeller_state = 0;
					//stop_climbing();
				}
			}
			break;
			
		case 2:
			//brushless_power_percent = BRUSHLESS_PUTTING_POWER;
			//brushless_control(brushless_power_percent, true);
			//brushless_servo_val = BRUSHLESS_SERVO_DEGREE;
			//brushless_servo_control(brushless_servo_val);
		
			if (putting_propeller_state == 0 && ((this_loop_ticks - putting_propeller_ticks[putting_propeller_state]) > PUTTING_PROPELLER_UP_DELAY)){
				slow_climb_continue();
				putting_propeller_state++;
				gripper_down = GRIPPER_FULL_UP;
				gripper_control(THIS_GRIPPER, gripper_down);
				putting_propeller_ticks[1] = this_loop_ticks;
				
			}else	if (putting_propeller_state == 1 && ((this_loop_ticks - putting_propeller_ticks[putting_propeller_state]) > PUTTING_PROPELLER_PUSH_DELAY)){
				stop_climbing();
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


#include "wheel_base.h"

static WHEEL_BASE_VEL wheel_base_vel = {0, 0, 0},
		wheel_base_vel_prev = {-1, -1, -1};
static const CLOSE_LOOP_FLAG wheel_base_close_loop_flag = CLOSE_LOOP;
static u8 wheel_base_speed_mode = WHEEL_BASE_DEFAULT_SPEED_MODE;
//static u32 wheel_base_bluetooth_vel_last_update = 0;
static char wheel_base_bluetooth_last_char = 0;
static u32 wheel_base_bluetooth_char_last_update = 0;
//static u32 wheel_base_last_can_tx = 0;
//static u32 wheel_base_joystick_vel_last_update=1;
static u8 wheel_base_joystick_speed=30;	//0% to 100%
//static u8 is_turning = 0, is_moving = 0;
static u8 wheel_base_pid_flag = 0;
static POSITION target_pos = {0, 0, 0};
//static PID wheel_base_pid = {0, 0, 0};

static WHEEL_BASE_VEL wheel_base_target = {0, 0, 0};

// Global variable for friction tunning
int accel_booster; // 1414 for rough ground, 1000 for 3142 ground without skidding (prescaled by 1000)

/**
	* @brief Handler for the bluetooth RX with id 0x4?
	* @param id: ID of the RX package
	* @param length: Data length
	* @param data: The array with the data length
	* @retval None
	*/
static void wheel_base_bluetooth_decode(u8 id, u8 length, u8* data)
{
	switch (id) {
		
		case BLUETOOTH_WHEEL_BASE_VEL_ID:
			wheel_base_pid_off();	//abort auto_positioning
			if (length == 3) {
				s8	x_vel = (s8)data[0],
						y_vel = (s8)data[1],
						w_vel = (s8)data[2];
				
				wheel_base_set_target_pos((POSITION){get_pos()->x, get_pos()->y,get_pos()->angle});
				
				s8 data_range[2] = {-100, 100};
				if (x_vel >= data_range[0] && x_vel <= data_range[1] &&
					y_vel >= data_range[0] && y_vel <= data_range[1] &&
					w_vel >= data_range[0] && w_vel <= data_range[1]) {
          
          u16 speed_ratio = SPEED_MODES[wheel_base_speed_mode]; 
					wheel_base_set_vel(x_vel * speed_ratio / 100, y_vel * speed_ratio / 100, w_vel * speed_ratio / 100);
					//wheel_base_bluetooth_vel_last_update = get_full_ticks();
				}
			}
		break;
			
		case BLUETOOTH_WHEEL_BASE_SPEED_MODE_ID:
			wheel_base_pid_off();	//abort auto_positioning
			if (length == 1) {
				if (data[0] <= 9) {
					wheel_base_set_speed_mode(data[0]);
				}
			}
		break;
			

	}
}

static void wheel_base_auto_bluetooth_decode(u8 id, u8 length, u8* data)
{
	switch (id) {
		case BLUETOOTH_WHEEL_BASE_AUTO_POS_ID:
			wheel_base_pid_flag=1;
			if (length == 6) {
				target_pos.x = ((data[0] << 8) & 0xFF00) | (data[1] & 0xFF);
				target_pos.y = ((data[2] << 8) & 0xFF00) | (data[3] & 0xFF);
				target_pos.angle = (((data[4] << 8) & 0xFF00) | (data[5] & 0xFF)) * 10;
			}
		break;
		
		case BLUETOOTH_WHEEL_BASE_AUTO_START_ID:
			if (length == 0) {
				wheel_base_pid_on();
			}
		break;

			
		case BLUETOOTH_WHEEL_BASE_AUTO_STOP_ID:
			wheel_base_pid_off();
		break;
	}
}

/**
  * @brief Handler for wheel base character decoding
  */
static void wheel_base_char_bluetooth_decode(u8 id, u8 length, u8* data)
{
  switch (id) {
    case BLUETOOTH_WHEEL_BASE_CHAR_ID:
      if (length == 1) {
        wheel_base_bluetooth_char_last_update = get_full_ticks();
        wheel_base_bluetooth_last_char = data[0];
      }
    break;
  }
}


/**
	* @brief Initialization of wheel base, including bluetooth rx filter, and all related variables
	*/
void wheel_base_init(void)
{
	bluetooth_rx_add_filter(BLUETOOTH_WHEEL_BASE_VEL_ID, 0xF0, wheel_base_bluetooth_decode);
  bluetooth_rx_add_filter(BLUETOOTH_WHEEL_BASE_AUTO_POS_ID, 0xF0, wheel_base_auto_bluetooth_decode);
  bluetooth_rx_add_filter(BLUETOOTH_WHEEL_BASE_CHAR_ID, 0xFF, wheel_base_char_bluetooth_decode);
	wheel_base_vel.x = wheel_base_vel.y = wheel_base_vel.w = 0;
	//wheel_base_bluetooth_vel_last_update = 0;
	//wheel_base_last_can_tx = 0;
	wheel_base_tx_acc();
	
	//PID on at beginning
	wheel_base_set_target_pos(wheel_base_get_target_pos());
	wheel_base_pid_on();
}

/**
	* @brief Set speed mode
	* @param The speed mode
	* @retval None
	*/
void wheel_base_set_speed_mode(u8 s)
{
	if (s <= 9) {
		wheel_base_speed_mode = s;
	}
}

/** 
	* @brief Get speed mode
	* @param None
	* @retval Get wheel base speed mode.
	*/
u8 wheel_base_get_speed_mode(void)
{
	return wheel_base_speed_mode;
}

/**
	* @brief Set acceleration
	* @param a: acceleration parameter (default 50)
	* @retval None.
	*/
void wheel_base_tx_acc(void)
{
	motor_set_acceleration(MOTOR_BACK_RIGHT,WHEEL_BASE_BR_ACC);
	motor_set_acceleration(MOTOR_BACK_LEFT,WHEEL_BASE_BL_ACC);
	motor_set_acceleration(MOTOR_FRONT,WHEEL_BASE_FR_ACC);
}


/**
	* @brief Set wheel base velocity TARGET ONLY!(NO CAN TRANSMISSION IN THIS FUNCTION!)
	* @param x:	Velocity in x direction (right as +ve).
	* @param y:	Velocity in y direction (up as +ve).
	* @param w:	Velocity in omega (clockwise as +ve).
	* @retval Velocity type value that has been set.
	*/
void wheel_base_set_vel(s32 x, s32 y, s32 w)
{
	wheel_base_target.x = x;
	wheel_base_target.y = y;
	wheel_base_target.w = w;
}

/**
	* @brief Get current wheel base velocity
	* @param None.
	* @retval Current wheel base velocity.
	*/
WHEEL_BASE_VEL wheel_base_get_vel(void)
{
	return wheel_base_vel;
}

/**
	* @brief Get wheel base target velocity
	* @param None.
	* @retval Target wheel base velocity.
	*/
WHEEL_BASE_VEL wheel_base_get_tar_vel(void)
{
	return wheel_base_target;
}

char wheel_base_bluetooth_get_last_char(void) 
{
   return wheel_base_bluetooth_last_char;
}

/**
	* @brief Check if the wheel base velocity is different from the previous one
	* @param None
	* @retval True if there is any different
	*/
u8 wheel_base_vel_diff(void)
{
	return wheel_base_vel.x != wheel_base_vel_prev.x || wheel_base_vel.y != wheel_base_vel_prev.y || wheel_base_vel.w != wheel_base_vel_prev.w;
}

/**
	*	@brief 	Update the velocity vector of x and y with respect to its direction. (private function)
	*	@param 	accumulate: Pointer of the vector of accumulated variable that used to simluate floating point
						target_vector: The vector of target velocity you set
						curr_vector: The vector of current velocity of the robot
						mag_of_vector_diff: The magnitude of vector difference
						curr_accel: Our acceleration rate (prescaled by 1000, by converting sec to ms)
						boost: True if boost acceleration due to friction, False if no need.
	*	@retval None
	*/
static void wheel_base_vector_update(s32* accumulate, const s32 target_vector, s32* curr_vector, const int mag_of_vector_diff, const int curr_accel, bool boost)
{
	const u16 ACCEL_PRESCALAR = 1000;	
	s32 adjusted_accel_rate = curr_accel;
	s32 adjusted_prescalar = ACCEL_PRESCALAR * mag_of_vector_diff;
	
	// Boost up accel rate if it is boosted
	if (boost) {
		adjusted_accel_rate =  curr_accel * accel_booster / 1000;
	}
	
	if (Abs(target_vector - *curr_vector) * ACCEL_PRESCALAR > adjusted_accel_rate) {
		// Increase or decrease current vector with respect to its magnitude by accel_booster value each milisecond.
		(*curr_vector) += ((target_vector - *curr_vector) * adjusted_accel_rate) / adjusted_prescalar;
		(*accumulate) += ((target_vector - *curr_vector) * adjusted_accel_rate) % adjusted_prescalar;
	} else {
		// Directly reach target vector if difference is small.
		(*curr_vector) = target_vector;
		(*accumulate) = 0;
		return;
	}
	
	// Add up the remainder value, just simulate floating-point like.
	if (Abs(*accumulate) > adjusted_prescalar) {
		int increment = (*accumulate) / adjusted_prescalar;
		(*accumulate) -= increment * adjusted_prescalar;
		(*curr_vector) += increment;
	}
}

/**
	* @brief Update the wheel base speed through CAN transmission. (TO BE CALLED REGULARLY)
	* @param None.
	* @retval None.
	*/
void wheel_base_update(void)
{
  /** 
    * TODO1: Use wheel_base_set_vel(x,y,w) to control the FOUR wheel base motor
    * TODO2: If there is not any Bluetooth RX data after BLUETOOTH_WHEEL_BASE_TIMEOUT, stop the motors
  
    */
	int mod = 1;
	
	// Acceleration profile relevant variable.
	static u16 prev_vels[50] = { 0 };
	static u16 vel_index = 0;
	// Floating point simulation variable.
	static WHEEL_BASE_VEL accumulate = {0, 0, 0};
	
	// Constant boolean for acceleration booster
	const bool xy_boost_accel = true;	// Boost up acceleration for x and y.
	const bool spin_no_boost_accel = false;	// NO Boost up acceleration for spining.

	
	// Acceleration profile, ranging from 50 to 400. (with 165 speed)
	if (get_ticks() % 10 == 0) {
		prev_vels[(vel_index++)%(sizeof(prev_vels)/sizeof(prev_vels[0]))] = (Sqrt(Sqr(Abs(wheel_base_get_tar_vel().x)) + Sqr(Abs(wheel_base_get_tar_vel().y))+ Sqr(Abs(wheel_base_get_tar_vel().w))))/20;
	}
	
	// Acceleration variable
	u16 acceleration = 0;		// xy acceleration
	const u16 alpha = 512;	// angular acceleration

	for (int i = 0; i < sizeof(prev_vels)/sizeof(prev_vels[0]); i++) {
		acceleration += (prev_vels[i] > 0 ? (prev_vels[i]) : 1);
	}
	
	// Accelerate according to the acceleration profile
	s32 mag_vector_diff = Sqrt(Sqr(wheel_base_get_vel().x - wheel_base_get_tar_vel().x) + Sqr(wheel_base_get_tar_vel().y - wheel_base_get_vel().y));
	s32 diff_omega = Abs(wheel_base_vel.w - wheel_base_get_tar_vel().w);
	
	// x, y accel with respect to magnitudue of vector difference.
	if (mag_vector_diff > 0) {
		wheel_base_vector_update(&(accumulate.x), wheel_base_get_tar_vel().x, &(wheel_base_vel.x), mag_vector_diff, acceleration, xy_boost_accel);
		wheel_base_vector_update(&(accumulate.y), wheel_base_get_tar_vel().y, &(wheel_base_vel.y), mag_vector_diff, acceleration, xy_boost_accel);
	}
	// Angular velocity acceleration
	if (diff_omega > 0) {
		wheel_base_vector_update(&(accumulate.w), wheel_base_get_tar_vel().w, &(wheel_base_vel.w), diff_omega, alpha, spin_no_boost_accel);
	}
	
	// Decide whether update to motor 
		motor_set_vel(MOTOR_BACK_RIGHT, (WHEEL_BASE_XY_VEL_RATIO * (wheel_base_vel.x + mod*wheel_base_vel.y) / 1000 + WHEEL_BASE_W_VEL_RATIO * wheel_base_vel.w / 1000), wheel_base_close_loop_flag);
		motor_set_vel(MOTOR_BACK_LEFT,  (WHEEL_BASE_XY_VEL_RATIO * (wheel_base_vel.x - mod*wheel_base_vel.y) / 1000 + WHEEL_BASE_W_VEL_RATIO * wheel_base_vel.w / 1000), wheel_base_close_loop_flag);
		motor_set_vel(MOTOR_FRONT,			(WHEEL_BASE_XY_VEL_RATIO * (-wheel_base_vel.x - wheel_base_vel.y) / 1000 + WHEEL_BASE_W_VEL_RATIO * wheel_base_vel.w / 1000), wheel_base_close_loop_flag);
	
	
	// Record the velocity.
	wheel_base_vel_prev.x = wheel_base_vel.x;
	wheel_base_vel_prev.y = wheel_base_vel.y;
	wheel_base_vel_prev.w = wheel_base_vel.w;
	
}

bool bluetooth_is_key_release(void)
{
  return ((get_full_ticks() - wheel_base_bluetooth_char_last_update) > 200 ? true : false);
}

/**
	* @brief Send the wheel base position (x, y, w) through Bluetooth
	* @param None.
	* @retval None.
	*/
void wheel_base_tx_position(void)
{
	s16 x = get_pos()->x,		
			y = get_pos()->y,				
			w = (get_pos()->angle / 10);
	
	u8 data[6];
	
	// POS XY
	data[0] = (x >> 8) & 0xFF;
	data[1] = x & 0xFF;
	data[2] = (y >> 8) & 0xFF;
	data[3] = y & 0xFF;
	data[4] = (w >> 8) & 0xFF;
	data[5] = w & 0xFF;	
	
	bluetooth_tx_package(BLUETOOTH_WHEEL_BASE_POS_ID, 6, data);
}


POSITION wheel_base_get_target_pos(void)
{
	return target_pos;
}

void wheel_base_set_target_pos(POSITION pos)
{
	target_pos = pos;
}

void wheel_base_pid_on(void)
{
	wheel_base_pid_flag = 1;
}

void wheel_base_pid_off(void)
{
	wheel_base_pid_flag = 0;
}

u8 wheel_base_get_pid_flag(void)
{
	return wheel_base_pid_flag;
}

void wheel_base_increase_joystick_speed(void)
{
	if (wheel_base_joystick_speed<100)
		wheel_base_joystick_speed+=10;
}

void wheel_base_decrease_joystick_speed(void)
{
	if (wheel_base_joystick_speed>0)
		wheel_base_joystick_speed-=10;
}

u8 wheel_base_get_joystick_speed(void)
{
	return wheel_base_joystick_speed;
}

#include "can_motor.h"

#define get_can_motor_id(motor_id)	(CAN_MOTOR_BASE + (u8)motor_id)
static s32 can_motor_encoder_value[CAN_MOTOR_COUNT] = {0};
static s32 can_motor_pwm_value[CAN_MOTOR_COUNT] = {0};

//To ignore message sent from the past
u16 last_received_ticks = 0;

void can_motor_init(void)
{
	can_rx_add_filter(get_can_motor_id(this_motor), CAN_RX_MASK_EXACT, 	motor_cmd_decoding);
}

/*** TX ***/
/**
	* @brief Send encoder value to mainboard (regular called in main)
	* @param None.
	* @retval None.
	*/
void send_encoder(s32 encoder_value){
	CAN_MESSAGE msg;
	msg.id = get_can_motor_id(this_motor);
	msg.length = CAN_ENCODER_FEEDBACK_LENGTH;
	msg.data[0] = CAN_ENCODER_FEEDBACK;
	msg.data[1] = one_to_n_bytes(encoder_value,0);
	msg.data[2] = one_to_n_bytes(encoder_value,1);
	msg.data[3] = one_to_n_bytes(encoder_value,2);
	msg.data[4] = one_to_n_bytes(encoder_value,3);
	can_tx_enqueue(msg);
}

void send_current_pwm(s32 current_pwm){
	CAN_MESSAGE msg;
	msg.id = get_can_motor_id(this_motor);
	msg.length = CAN_PWM_FEEDBACK_LENGTH;
	msg.data[0] = CAN_PWM_FEEDBACK;
	msg.data[1] = one_to_n_bytes(current_pwm,0);
	msg.data[2] = one_to_n_bytes(current_pwm,1);
	msg.data[3] = one_to_n_bytes(current_pwm,2);
	msg.data[4] = one_to_n_bytes(current_pwm,3);
	can_tx_enqueue(msg);
}

void send_target_vel(s32 target_vel){
	CAN_MESSAGE msg;
	msg.id = get_can_motor_id(this_motor);
	msg.length = CAN_TARGET_VEL_FEEDBACK_LENGTH;
	msg.data[0] = CAN_TARGET_VEL_FEEDBACK;
	msg.data[1] = one_to_n_bytes(target_vel,0);
	msg.data[2] = one_to_n_bytes(target_vel,1);
	msg.data[3] = one_to_n_bytes(target_vel,2);
	msg.data[4] = one_to_n_bytes(target_vel,3);
	can_tx_enqueue(msg);
}

void send_curr_vel(s32 curr_vel){
	CAN_MESSAGE msg;
	msg.id = get_can_motor_id(this_motor);
	msg.length = CAN_CURR_VEL_FEEDBACK_LENGTH;
	msg.data[0] = CAN_CURR_VEL_FEEDBACK;
	msg.data[1] = one_to_n_bytes(curr_vel,0);
	msg.data[2] = one_to_n_bytes(curr_vel,1);
	msg.data[3] = one_to_n_bytes(curr_vel,2);
	msg.data[4] = one_to_n_bytes(curr_vel,3);
	can_tx_enqueue(msg);
}

/*** User Interface ***/
/** These are how mainboard sent data control motor, keep here for reference ONLY **/
/**
	* @brief Set motor velocity (CAN)
	* @param motor_id (MOTOR_ID enum)
	* @param vel (vel of close_loop is not corresponded to open_loop)
	* @param close_loop_flag: true if close_loop should be applied
	* @retval None.
	*/
void motor_set_vel(MOTOR_ID motor_id, s32 vel, CLOSE_LOOP_FLAG close_loop_flag)
{
	CAN_MESSAGE msg;
	
	assert_param((u8)motor_id < CAN_MOTOR_COUNT);

	msg.id = get_can_motor_id(motor_id);
	msg.length = CAN_MOTOR_VEL_LENGTH;
	msg.data[0] = CAN_MOTOR_VEL_CMD;
	msg.data[1] = (u8)(one_to_n_bytes(vel, 0));
	msg.data[2] = (u8)(one_to_n_bytes(vel, 1));
	msg.data[3] = (u8)(one_to_n_bytes(vel, 2));
	msg.data[4] = (u8)(one_to_n_bytes(vel, 3));
	msg.data[5] = (u8)(close_loop_flag);
	
	can_tx_enqueue(msg);
}

/**
	* @brief Set motor position (CAN)
	* @param motor_id (MOTOR_ID enum)
	* @param vel (vel of close_loop is not corresponded to open_loop)
	* @param pos: The position need to move to relative to current encoder value.
	* @retval None.
	*/
void motor_set_pos(MOTOR_ID motor_id, u16 vel, s32 pos)
{
	CAN_MESSAGE msg;
	
	assert_param((u8)motor_id < CAN_MOTOR_COUNT);
	
	msg.id = get_can_motor_id(motor_id);
	msg.length = CAN_MOTOR_POS_LENGTH;
	msg.data[0] = CAN_MOTOR_POS_CMD;
	msg.data[1] = (u8)(one_to_n_bytes(vel, 0));
	msg.data[2] = (u8)(one_to_n_bytes(vel, 1));
	msg.data[3] = (u8)(one_to_n_bytes(pos, 0));
	msg.data[4] = (u8)(one_to_n_bytes(pos, 1));
	msg.data[5] = (u8)(one_to_n_bytes(pos, 2));
	msg.data[6] = (u8)(one_to_n_bytes(pos, 3));

	can_tx_enqueue(msg);
}

/**
	* @brief Set motor acceleration (CAN)
	* @param motor_id (MOTOR_ID enum)
	* @param accel: acceleration parameter of motor
	* @retval None.
	*/
void motor_set_acceleration(MOTOR_ID motor_id, u16 accel)
{
	CAN_MESSAGE msg;
	
	assert_param((u8)motor_id < CAN_MOTOR_COUNT);
	
	msg.id = get_can_motor_id(motor_id);
	msg.length = CAN_MOTOR_ACCEL_LENGTH;
	msg.data[0] = CAN_MOTOR_ACCEL_CMD;
	msg.data[1] = (u8)(one_to_n_bytes(accel, 0));
	msg.data[2] = (u8)(one_to_n_bytes(accel, 1));

	can_tx_enqueue(msg);
}

/**
	* @brief Lock and stop motor immediately (CAN)
	* @param motor_id (MOTOR_ID enum)
	* @retval None.
	*/
void motor_lock(MOTOR_ID motor_id)
{
	CAN_MESSAGE msg;
	
	assert_param((u8)motor_id < CAN_MOTOR_COUNT);
	
	msg.id = get_can_motor_id(motor_id);
	msg.length = CAN_MOTOR_LOCK_LENGTH;
	msg.data[0] = CAN_MOTOR_LOCK_CMD;

	can_tx_enqueue(msg);
}
/*** End of TX ***/

/*** RX ***/

/*** motor command decoding ***/
void motor_cmd_decoding(CanRxMsg msg)
{
	switch (msg.Data[0]) {
		case CAN_MOTOR_VEL_CMD:
			if (msg.DLC == CAN_MOTOR_VEL_LENGTH) {
				const u8 VEL_SIZE = 4;	// velocity data contain 4 bytes
				u8 fragment_vel[VEL_SIZE] = {0};
				for (u8 i = 0; i < VEL_SIZE; ++i) {
					fragment_vel[i] = msg.Data[i+1];
				}
				CLOSE_LOOP_FLAG loop_flag = (CLOSE_LOOP_FLAG) msg.Data[5];// 5-th byte is loop-flag (start as 0th byte)
				// velocity or pwm control.
				s32 velocity = n_bytes_to_one(fragment_vel, VEL_SIZE);
				// Ignore if same velocity is sent.
				if (velocity != get_target_vel() || loop_flag == OPEN_LOOP) {
					(loop_flag == CLOSE_LOOP) ? set_velocity(velocity) : set_pwm(velocity);
				}
			}
			break;
		case CAN_MOTOR_VEL_STAMP_CMD:
			if (msg.DLC == CAN_MOTOR_VEL_STAMP_LENGTH) {
				u16 received_ticks = n_bytes_to_one(&msg.Data[5], 2);
				
				//Receiving message from the past.. Ignore it
				if ((last_received_ticks > received_ticks+5)){
					break;
				}else{
					last_received_ticks = received_ticks;
				}
				
				const u8 VEL_SIZE = 4;	// velocity data contain 4 bytes
				u8 fragment_vel[VEL_SIZE] = {0};
				for (u8 i = 0; i < VEL_SIZE; ++i) {
					fragment_vel[i] = msg.Data[i+1];
				}
				CLOSE_LOOP_FLAG loop_flag = (CLOSE_LOOP_FLAG) msg.Data[7];// 7-th byte is loop-flag (start as 0th byte)
				// velocity or pwm control.
				s32 velocity = n_bytes_to_one(fragment_vel, VEL_SIZE);
				// Ignore if same velocity is sent.
				if (velocity != get_target_vel() || loop_flag == OPEN_LOOP) {
					(loop_flag == CLOSE_LOOP) ? set_velocity(velocity) : set_pwm(velocity);
				}
			}
			break;
		case CAN_MOTOR_ACCEL_CMD:
			if (msg.DLC == CAN_MOTOR_ACCEL_LENGTH) {
				const u8 ACCEL_SIZE = 2;	// acceleration data contain 2 bytes
				u8 fragment_accel[ACCEL_SIZE] = {0};
				for (u8 i = 0; i < ACCEL_SIZE; ++i) {
					fragment_accel[i] = msg.Data[i+1];
				}
				set_acceleration(n_bytes_to_one(fragment_accel, ACCEL_SIZE));
			}
			break;
		case CAN_MOTOR_POS_CMD:
			if (msg.DLC == CAN_MOTOR_POS_LENGTH) {
				const u8 SPD_SIZE = 2;
				const u8 POS_SIZE = 4;
				u8 fragment_pos[POS_SIZE] = {0};
				u8 fragment_spd[SPD_SIZE] = {0};
				for (u8 i = 0; i < POS_SIZE; ++i) {
					if (i < SPD_SIZE) {
						fragment_pos[i] = msg.Data[i+1];
					}
					fragment_pos[i] = msg.Data[i+3];
				}
				set_pos(n_bytes_to_one(fragment_pos, POS_SIZE), n_bytes_to_one(fragment_spd, SPD_SIZE));
			}
			break;
		case CAN_MOTOR_LOCK_CMD:
			if (msg.DLC == CAN_MOTOR_LOCK_LENGTH) {
				lock_motor();
			}
			break;
		case CAN_MOTOR_LOCK_STAMP_CMD:
			if (msg.DLC == CAN_MOTOR_LOCK_STAMP_LENGTH) {
				lock_motor();
			}
			s32 received_ticks = n_bytes_to_one(&msg.Data[1], 2);
			//Receiving message from the past.. Ignore it
			if ((last_received_ticks - received_ticks)>0){
				break;
			}else{
				received_ticks = last_received_ticks;
			}
			break;
		default:
			return;
	}
}

/*** User interface ***/
/** These are how mainboard receive data, keep here for reference ONLY **/

//void can_motor_feedback(CanRxMsg msg){
//	switch (msg.Data[0]) {
//		case CAN_ENCODER_FEEDBACK:
//			if (msg.DLC == CAN_ENCODER_FEEDBACK_LENGTH) {
//				// Range check 
//				if (msg.StdId >= CAN_MOTOR_BASE && msg.StdId < CAN_MOTOR_BASE + CAN_MOTOR_COUNT) {
//					s32 feedback = n_bytes_to_one(&msg.Data[1], 4);
//					can_motor_encoder_value[msg.StdId - CAN_MOTOR_BASE] = feedback;
//				}
//			}
//		break;
//		case CAN_PWM_FEEDBACK:
//			if (msg.DLC == CAN_PWM_FEEDBACK_LENGTH) {
//				// Range check 
//				if (msg.StdId >= CAN_MOTOR_BASE && msg.StdId < CAN_MOTOR_BASE + CAN_MOTOR_COUNT) {
//					s32 feedback = n_bytes_to_one(&msg.Data[1], 4);
//					can_motor_pwm_value[msg.StdId - CAN_MOTOR_BASE] = feedback;
//				}
//			}
//		break;
//		case CAN_VEL_FEEDBACK:
//			if (msg.DLC == CAN_VEL_FEEDBACK_LENGTH) {
//				// Range check 
//				if (msg.StdId >= CAN_MOTOR_BASE && msg.StdId < CAN_MOTOR_BASE + CAN_MOTOR_COUNT) {
//					s32 feedback = n_bytes_to_one(&msg.Data[1], 4);
//					can_motor_vel_value[msg.StdId - CAN_MOTOR_BASE] = feedback;
//				}
//			}
//		break;
//	}
//}



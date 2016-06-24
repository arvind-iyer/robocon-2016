#include "gyro.h"
#include "approx_math.h"

s16 angle = 0, real_x = 0, real_y = 0;
s16 angle_offset = 0, x_offset = 0, y_offset = 0;

//GEN2 hybrid val
s32 SHIFT_X = -61;
s32 SHIFT_Y = -50;

static POSITION gyro_pos = {0, 0, 0};
static POSITION gyro_pos_raw = {0, 0, 0};
static u8 rx_state = 0; 
static u8 rx_command = 0;
static u8 buf_rec = 0;
static u8 buf_data[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static u8 rx_command_arr[GYRO_COMMAND_LENGTH] = {GYRO_UPDATED, GYRO_REPLY};
static u8 buf_len[GYRO_COMMAND_LENGTH] = {0x06, 0x01};		//data size, for confirm data

volatile u8 reply_flag = 0;

volatile u8 gyro_available = 0;


void plus_x(void)
{
	SHIFT_X++;
}
void minus_x(void)
{
	SHIFT_X--;
}
void plus_y(void)
{
	SHIFT_Y++;
}
void minus_y(void)
{
	SHIFT_Y--;
}

s32 gyro_get_shift_x(void)
{
		return SHIFT_X;
}

s32 gyro_get_shift_y(void)
{
	return SHIFT_Y;
}
/**
  * @brief  Initialization of Gyro
  * @param  None
  * @retval None
  */
void gyro_init(void)
{
	uart_init(GYRO_UART, 115200);
	uart_interrupt(GYRO_UART);
}


/**
	* @brief Get the position object
	* @param None
	* @retval The position object
	*/
const POSITION* get_pos(void)
{
	return &gyro_pos;
}

const POSITION* get_pos_raw(void)
{
  return &gyro_pos_raw;
}

/**
  * @brief  Get the Angle (yaw)
  * @param  None
  * @retval Angle (yaw)
  */
s16 get_angle(void)
{
	return gyro_pos.angle;
}

/**
  * @brief  Send the update flag
  * @param  None
  * @retval None
  */
void gyro_pos_update(void)			// unuseful
{
	uart_tx_byte(GYRO_UART, GYRO_WAKEUP);
	uart_tx_byte(GYRO_UART, GYRO_UPDATE);
	uart_tx_byte(GYRO_UART, 0);
}

/**
  * @brief  Send the calibration flag
  * @param  None
  * @retval 1 = successful, 0 = failed
  */
u8 gyro_cal(void)
{
	u16 ticks_last = get_ticks();
	reply_flag &= ~GYRO_FLAG_CAL;
	
	uart_tx_byte(GYRO_UART, GYRO_WAKEUP);
	uart_tx_byte(GYRO_UART, GYRO_CAL);
	uart_tx_byte(GYRO_UART, 0);
	
	while (!(reply_flag & GYRO_FLAG_CAL)) {
		if ((get_ticks()+1000-ticks_last) % 1000 >= 20)			// 20 ms timeout
			return 0;
	}
	return 1;
}

/**
  * @brief  Update the offset of X, Y coordinate and Angle
  * @param  x: X coordinate to be set
  * @param  y: Y coordinate to be set
  * @param  a: angle to be set
  * @retval 1 = successful, 0 = failed
  */
u8 gyro_pos_set(s16 x, s16 y, s16 a)
{
	u16 ticks_last = get_ticks();
	reply_flag &= ~GYRO_FLAG_SET_POS;
	
	uart_tx_byte(GYRO_UART, GYRO_WAKEUP);
	uart_tx_byte(GYRO_UART, GYRO_POS_SET);
	uart_tx_byte(GYRO_UART, 0x06);
	uart_tx_byte(GYRO_UART, x >> 8);
	uart_tx_byte(GYRO_UART, x & 0xFF);
	uart_tx_byte(GYRO_UART, y >> 8);
	uart_tx_byte(GYRO_UART, y & 0xFF);
	uart_tx_byte(GYRO_UART, a >> 8);
	uart_tx_byte(GYRO_UART, a & 0xFF);
	
	u16 timeout = 100;
	while (!(reply_flag & GYRO_FLAG_SET_POS)) {
		if (!(--timeout)) {
			return 0;
		}
	}

	return 1;
}


s16 get_X(void)
{
   s32 pos_x = (real_x*10000-SHIFT_X*10000+SHIFT_X*int_cos(angle)+SHIFT_Y*int_sin(angle))/10000;
	//return real_x;
	return pos_x;//real_x;
}

/**
  * @brief  Get the Y coordinate
  * @param  None
  * @retval Y coordinate
  */
s16 get_Y(void)
{
	s32 pos_y = (real_y*10000-SHIFT_Y*10000+SHIFT_Y*int_cos(angle)-SHIFT_X*int_sin(angle))/10000;
	//return real_y;
	return pos_y;//real_y;
}

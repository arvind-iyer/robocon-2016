#include "can_xbc_mb.h"

static u32 xbc_digital = 0;
static s16 xbc_joy[XBC_JOY_COUNT] = {0};
static u32 last_can_connection = 0;
static CAN_XBC_CONNECTION_MODE xbc_connection = CAN_XBC_DISCONNECTED;
static bool xbc_mb_tx_flag = false;

static XBC_LCD_DATA xbc_lcd_data[CHAR_MAX_X_VERTICAL][CHAR_MAX_Y_VERTICAL],
  xbc_lcd_data_prev[CHAR_MAX_X_VERTICAL][CHAR_MAX_Y_VERTICAL];

static void can_xbc_mb_decoding(CanRxMsg msg)
{
    switch (msg.StdId) {
      case CAN_XBC_BASE:
          if (msg.DLC == 8) {
            xbc_digital = msg.Data[0] + (msg.Data[1] << 8);
            xbc_joy[XBC_JOY_LT] = msg.Data[2];
            xbc_joy[XBC_JOY_RT] = msg.Data[3];
            xbc_joy[XBC_JOY_LX] = msg.Data[4] + (msg.Data[5] << 8); 
            xbc_joy[XBC_JOY_LY] = msg.Data[6] + (msg.Data[7] << 8);
            xbc_connection = CAN_XBC_ALL_CONNECTED;
          } else {
            // Disconnected: reset all variables
            xbc_digital = 0;
            for (u8 i = 0; i < XBC_JOY_COUNT; ++i) {
              xbc_joy[i] = 0;
            }
            xbc_connection = CAN_XBC_CAN_CONNECTED;
          }
          last_can_connection = get_full_ticks();
      break;
      
      case CAN_XBC_BASE+1:
        if (msg.DLC == 5) {
          xbc_joy[XBC_JOY_RX] = msg.Data[0] + (msg.Data[1] << 8); 
          xbc_joy[XBC_JOY_RY] = msg.Data[2] + (msg.Data[3] << 8);
          last_can_connection = get_full_ticks();
          xbc_connection = CAN_XBC_ALL_CONNECTED;
        }
      break;
    }
}



/**
  * @brief Get the can xbc connection state
  * @retval @ref XBC_CONNECTION_MODE
  */
CAN_XBC_CONNECTION_MODE can_xbc_get_connection(void)
{
  if (get_full_ticks() - last_can_connection > CAN_XBC_CONNECTION_TIMEOUT_MS) {
    xbc_connection = CAN_XBC_DISCONNECTED;
  }
  return xbc_connection;
}

/**
  * @brief Get the xbc button in digital (used with bitwise operator)
  * @warning return 0 when xbc is disconnected
  */
u32 can_xbc_get_digital(void)
{
  if (can_xbc_get_connection() == CAN_XBC_DISCONNECTED) {
    return 0;
  }
  return xbc_digital;
}

/**
  * @brief Get the joy stick value (raw, from the xbc)
  * @param j: (XBC_JOY) Joy stick id 
  * @retval The raw joy stick value (-32768 to 32767 for 2-byte; 0 to 255 for 1-byte) 
  */
s16 can_xbc_get_joy_raw(XBC_JOY j) 
{
  if (can_xbc_get_connection() == CAN_XBC_DISCONNECTED) {
    return 0;
  }
  
  return xbc_joy[j];
}

/**
  * @brief Get the joy stick value (deadzone corrected for the 2-byte joystick)
  * @param j: (XBC_JOY) Joy stick id
  * @retval The joy stick value with deadzone (-XBC_JOY_DEADZONE_MIN to XBC_JOY_DEADZONE_MIN for 2-byte, 0 to 255 for 1-byte)
  */
s16 can_xbc_get_joy(XBC_JOY j)
{
  if (can_xbc_get_connection() == CAN_XBC_DISCONNECTED) { 
    return 0;
  }
  
  switch (j) {
    case XBC_JOY_LT:
    case XBC_JOY_RT:
      return xbc_joy[j];

    
    case XBC_JOY_LX:
    case XBC_JOY_LY:
    case XBC_JOY_RX:
    case XBC_JOY_RY:
      if (xbc_joy[j] >= -XBC_JOY_DEADZONE_MIN && xbc_joy[j] <= XBC_JOY_DEADZONE_MIN) {
        return 0;
      } else if (xbc_joy[j] < -XBC_JOY_DEADZONE_MAX) {
        return -XBC_JOY_SCALE;
      } else if (xbc_joy[j] > XBC_JOY_DEADZONE_MAX) {
        return XBC_JOY_SCALE;
      } else {
        if (xbc_joy[j] > 0) {
          return (xbc_joy[j] - XBC_JOY_DEADZONE_MIN) * XBC_JOY_SCALE / (XBC_JOY_DEADZONE_MAX - XBC_JOY_DEADZONE_MIN);
        } else {
          return (xbc_joy[j] - -XBC_JOY_DEADZONE_MIN) * XBC_JOY_SCALE / (XBC_JOY_DEADZONE_MAX - XBC_JOY_DEADZONE_MIN);
        }
      }
      
  }
  return 0;
}

/**
  * @brief CAN Xbc to MB protocol initializatoin 
  */
void can_xbc_mb_init(void) 
{
    xbc_connection = CAN_XBC_DISCONNECTED;
    xbc_digital = 0;
    can_rx_add_filter(CAN_XBC_BASE, CAN_RX_MASK_DIGIT_0_3, can_xbc_mb_decoding);
    for (u8 x = 0; x < CHAR_MAX_X_VERTICAL; ++x) {
      for (u8 y = 0; y < CHAR_MAX_Y_VERTICAL; ++y) {
        XBC_LCD_DATA* data = &xbc_lcd_data[x][y];
        XBC_LCD_DATA* data_prev = &xbc_lcd_data_prev[x][y];
        data->color = 0xFFFF; // WHITE 
        data->bg_color = 0x0000;  // BLACK
        
        data_prev->color = 0x0000;
        data_prev->bg_color = 0xFFFF;
      }
    }
    
    tft_update_trigger(can_xbc_mb_lcd_tx);
}

void can_xbc_mb_tx_enable(bool flag)
{
  xbc_mb_tx_flag = flag;  
}

static u8 xbc_lcd_data_diff(XBC_LCD_DATA* data1, XBC_LCD_DATA* data2) 
{
  if (data1 == 0 || data2 == 0) {
    return 0;
  } else {
    return data1->color != data2->color || data1->bg_color != data2->bg_color || data1->text != data2->text;
  }
}

static u8 xbc_lcd_data_color_diff(XBC_LCD_DATA* data1, XBC_LCD_DATA* data2)
{
  if (data1 == 0 || data2 == 0) {
    return 0;
  } else {
    return (data1->color != data2->color) || (data1->bg_color != data2->bg_color);
  }
}


/**
  * @brief Transmit Mainboard LCD data to XBC
  */
void can_xbc_mb_lcd_tx(void)
{
  if (!xbc_mb_tx_flag) {return;}
  bool package_tx_pending = false;
  u8 text_in_package = 0;
  CAN_MESSAGE msg;
  msg.id = CAN_XBC_MB_TX_LCD_ID;
  msg.length = 2;
  
  XBC_LCD_DATA* last_tx_data = 0;
  
  for (u8 y = 0; y < CHAR_MAX_Y_VERTICAL; ++y) {
    for (u8 x = 0; x < CHAR_MAX_X_VERTICAL; ++x) {
      XBC_LCD_DATA* data = &xbc_lcd_data[x][y];
      XBC_LCD_DATA* data_prev = &xbc_lcd_data_prev[x][y];
      
      data->color = text_color[x][y];
      data->bg_color = bg_color[x][y];
      data->text = text[x][y];
      

      
      // TEXT OF THE SAME COLOR AND SAME BG_COLOR WILL BE PACKED INSIDE THE SAME PACKAGE
      if (text_in_package == 0 || msg.length >= 8 || xbc_lcd_data_color_diff(last_tx_data, data)) {
        if (package_tx_pending) {
          package_tx_pending = false;
          can_tx_enqueue(msg); 
          msg.length = 2;
        }
              
        text_in_package = 0;
        msg.id = (CAN_XBC_MB_TX_LCD_ID & 0xF00) | ((x << 4) & 0xF0) | (y & 0x0F);
        /*** DATA[0] - COLOR in RGB232***/
        msg.data[0] = RGB565TORGB323(data->color);
        /*** DATA[2] - BG_COLOR in RGB232 ***/
        msg.data[1] = RGB565TORGB323(data->bg_color);
        
      } 
      
      last_tx_data = data;
      
      msg.data[2 + text_in_package] = data->text;
      ++msg.length;
      ++text_in_package;
      package_tx_pending = true;
      
    }
  }
  
  if (package_tx_pending) {
    package_tx_pending = false;
    can_tx_enqueue(msg); 
  }
  
}
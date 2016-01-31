//#ifndef __LCD_RED_H
//#define __LCD_RED_H

//#include <stdio.h>
//#include <stdarg.h>
//#include "stm32f4xx_hal.h"
//#include "stm32f4xx_hal_gpio.h"
//#include "stm32f4xx_hal_i2s.h"
//#include "cmsis_os.h"


//// SPI, RST, DC
//#define TFT_RST_PIN		GPIO_PIN_8
//#define TFT_DC_PIN		GPIO_PIN_12
//#define TFT_RST_PORT	GPIOD
//#define TFT_DC_PORT		GPIOB

////CS
//#define TFT_SPI			SPI2
//#define GPIO_PIN_CS		GPIO_PIN_9
//#define RCC_APB2Periph_GPIO_CS	RCC_APB2Periph_GPIOD
//#define GPIO_CS			GPIOD

//// Color
//#define WHITE			0xFFFF
//#define BLACK			0x0000
//#define DARK_GREY		0x5A8B
//#define GREY			0xC718
//#define BLUE			0x001F
//#define BLUE2			0x051F
//#define RED				0xF800
//#define MAGENTA			0xF81F
//#define GREEN			0x07E0
//#define CYAN			0x7FFF
//#define YELLOW			0xFFE0

//#define MAX_WIDTH				128
//#define MAX_HEIGHT				160
//#define CHAR_WIDTH				8
//#define CHAR_HEIGHT				16

//#define CHAR_MAX_X_VERTICAL		16
//#define CHAR_MAX_Y_VERTICAL		10

//#define CHAR_MAX_X_HORIZONTAL	20
//#define CHAR_MAX_Y_HORIZONTAL	8

//#define CHAR_MAX_X				20		// max between CHAR_MAX_X_VERTICAL and CHAR_MAX_X_HORIZONTAL
//#define CHAR_MAX_Y				10		// max between CHAR_MAX_Y_VERTICAL and CHAR_MAX_Y_HORIZONTAL

//extern uint8_t tft_orientation;
//extern uint8_t tft_width;
//extern uint8_t tft_height;
//extern uint16_t curr_bg_color;
//extern uint16_t curr_text_color;
//extern uint16_t curr_text_color_sp;

//extern char text[CHAR_MAX_X][CHAR_MAX_Y];
//extern uint16_t text_color[CHAR_MAX_X][CHAR_MAX_Y];
//extern uint16_t bg_color[CHAR_MAX_X][CHAR_MAX_Y];
//extern uint8_t text_bg_color_prev[CHAR_MAX_X][CHAR_MAX_Y]; // for transmit for xbc, msb 4bits: text color, lsb 4bits: bg color

//void tft_spi_init(void);
//void tft_write_command(uint8_t command);
//void tft_write_data(uint8_t data);
//void tft_config(void);
//void tft_reset(void);

//void tft_init(uint8_t orientation, uint16_t in_bg_color, uint16_t in_text_color, uint16_t in_text_color_sp);
//void tft_enable(void);
//void tft_disable(void);
//void tft_set_bg_color(uint16_t in_bg_color);
//void tft_set_text_color(uint16_t in_text_color);
//void tft_set_special_color(uint16_t text_color_sp);
//void tft_set_pixel_pos(uint8_t x, uint8_t y);
//void tft_set_char_pos(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
//void tft_force_clear(void);
//void tft_clear_line(uint8_t line);
//void tft_clear(void);
//void tft_toggle(void);
//void tft_put_pixel(uint8_t x, uint8_t y, uint16_t color);
//void tft_fill_color(uint16_t color);
//uint8_t tft_char_is_changed(uint8_t x, uint8_t y);
//void tft_prints(uint8_t x, uint8_t y, const char * pstr, ...);
//void tft_update(void);

//#endif		/* __LCD_RED_H */

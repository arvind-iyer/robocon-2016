#ifndef __LCD_RED_H
#define __LCD_RED_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

#include <stdio.h>
#include <stdarg.h>

#include "delay.h"

// SPI, RST, DC
#define TFT_RST_PIN		GPIO_Pin_7
#define TFT_DC_PIN		GPIO_Pin_6
#define TFT_RST_PORT	GPIOD
#define TFT_DC_PORT		GPIOD

#define TFT_SPI			SPI3
#define GPIO_Pin_CS		GPIO_Pin_5
#define RCC_APB2Periph_GPIO_CS	RCC_APB2Periph_GPIOD
#define GPIO_CS			GPIOD
// Color
#define	BGR888_MODE		1

#if (!BGR888_MODE)
#define	RGB888TO565(RGB888)  (((RGB888 >> 8) & 0xF800) |((RGB888 >> 5) & 0x07E0) | ((RGB888 >> 3) & 0x001F))
#else 
#define	RGB888TO565(BGR888)  (((BGR888 >> 19) & 0x001F) |((BGR888 >> 5) & 0x07E0) | (((u32)BGR888 << 8) & 0xF800))
#endif

//to minimize the MCU calculation
#define WHITE               (RGB888TO565(0xFFFFFF))
#define BLACK               (RGB888TO565(0x000000))
#define DARK_GREY           (RGB888TO565(0x555555))
#define GREY                (RGB888TO565(0xAAAAAA))
#define RED                 (RGB888TO565(0xFF0000))
#define DARK_RED            (RGB888TO565(0x800000))
#define ORANGE              (RGB888TO565(0xFF9900))
#define YELLOW              (RGB888TO565(0xFFFF00))
#define GREEN               (RGB888TO565(0x00FF00))
#define DARK_GREEN          (RGB888TO565(0x00CC00))
#define BLUE                (RGB888TO565(0x0000FF))
#define BLUE2               (RGB888TO565(0x202060))
#define SKY_BLUE            (RGB888TO565(0x11CFFF))
#define CYAN                (RGB888TO565(0x8888FF))
#define PURPLE              (RGB888TO565(0x00AAAA))
#define PINK                (RGB888TO565(0xFFB6C1))

#define MAX_WIDTH				128
#define MAX_HEIGHT				160
#define CHAR_WIDTH				8
#define CHAR_HEIGHT				16

#define CHAR_MAX_X_VERTICAL		15
#define CHAR_MAX_Y_VERTICAL		9

#define CHAR_MAX_X_HORIZONTAL	19
#define CHAR_MAX_Y_HORIZONTAL	7

#define CHAR_MAX_X_ANY	19
#define CHAR_MAX_Y_ANY	9

extern u8 tft_orientation;
extern u8 tft_width;
extern u8 tft_height;
extern u16 curr_bg_color;
extern u16 curr_text_color;
extern u16 curr_text_color_sp;

extern char text						[CHAR_MAX_X_ANY+1][CHAR_MAX_Y_ANY+1];
extern u16 text_color				[CHAR_MAX_X_ANY+1][CHAR_MAX_Y_ANY+1];
extern u16 bg_color					[CHAR_MAX_X_ANY+1][CHAR_MAX_Y_ANY+1];
extern u8 text_bg_color_prev[CHAR_MAX_X_ANY+1][CHAR_MAX_Y_ANY+1]; // for transmit for xbc, msb 4bits: text color, lsb 4bits: bg color

typedef enum {
	PIN_ON_TOP = 0,
	PIN_ON_LEFT = 1,
	PIN_ON_BOTTOM = 2,
	PIN_ON_RIGHT = 3
}TFT_ORIENTATION;

void tft_spi_init(void);
void tft_write_command(u8 command);
void tft_write_data(u8 data);
void tft_config(void);
void tft_reset(void);

void tft_init(TFT_ORIENTATION orientation, u16 bg_color, u16 text_color, u16 sp_color);
void tft_easy_init();
void tft_enable(void);
void tft_disable(void);
void tft_set_bg_color(u16 in_bg_color);
void tft_set_text_color(u16 in_text_color);
void tft_set_special_color(u16 text_color_sp);
void tft_set_pixel_pos(u8 x, u8 y);
void tft_set_char_pos(u8 x1, u8 y1, u8 x2, u8 y2);
void tft_force_clear(void);
void tft_clear_line(u8 line);
void tft_clear(void);
void tft_put_pixel(u8 x, u8 y, u16 color);
void tft_fill_color(u16 color);
u8 tft_char_is_changed(u8 x, u8 y);
void tft_prints(u8 x, u8 y, const char * pstr, ...);
void tft_update(void);
u8 tft_get_orientation(void);
void tft_append_line(const char * pstr, ...);
void tft_stream(const char * pstr, ...);

void tft_put_mega_ass_num(u8 x, u8 y, u8 character, u16 color);
void tft_mega_update();
void tft_put_logo(u8 x, u8 y);

#endif		/* __LCD_RED_H */
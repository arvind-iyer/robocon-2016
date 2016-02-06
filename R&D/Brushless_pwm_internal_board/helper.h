#pragma once

/*
 * helper.h
 *
 * Author: Petel__
 * Copyright (c) 2014-2015 HKUST Robotics Team
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 
#include <assert.h>
#include "stm32f10x.h"
#include "ticks.h"
#include "LEDs.h"
#include "tft_display.h"
#include "adc.h"
#include "linear_ccd.h"
#include "buzzer.h"
#include "buzzer_song.h"
#include "servo_tutorial.h"
#include "motor.h"
#include "GPIO_switch.h"
#include "button.h"
#include "uart.h"

// LCD Config
#define LCD_orientation_index						0x00
#define LCD_background_color						BLACK
#define LCD_text_color									WHITE
#define LCD_special_color								RED

// UART Config
#define UART_port												COM3
#define UART_buadrate										115200
#define UART_printf_port								COM1

// Buzzer Config
#define BUZZER_volume										50

// Util
#define ABS(v)													((v >= 0)? v : -v)

extern volatile uint16_t lastSystemPollingTime;
extern volatile uint16_t lastSystemPollingCounter;

void init(void);
void systemPolling(void);

void failLoop(uint16_t freq, GPIO_TypeDef* PORT,u16 gpio_pin);

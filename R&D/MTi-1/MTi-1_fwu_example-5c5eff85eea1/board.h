/*!
 * \file
 * \copyright Copyright (C) Xsens Technologies B.V., 2015.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
#ifndef BOARD_H
#define BOARD_H
#include "mbed.h"

#if defined(TARGET_NUCLEO_F401RE)

#define MT_DRDY				PB_3
#define MT_RESET			PC_9

// Uart connection with the PC:
#define PC_TX				PA_2
#define PC_RX				PA_3

// SPI connection with the module:
#define MT_SCLK				PB_13
#define MT_MISO				PB_14
#define MT_MOSI				PB_15
#define MT_nCS				PB_6

// I2C connection with the module:
#define MT_SCL				PB_8
#define MT_SDA				PB_9
#define MT_ADD0				PB_13
#define MT_ADD1				PB_14
#define MT_ADD2				PB_15

// UART connection with the module:
#define UART_TX_TO_MODULE   PA_9
#define UART_RX_FROM_MODULE PA_10

#else
#error "Support for selected mbed platform has not been added."
#endif // TARGET_NUCLEO_F401RE

#endif // BOARD_H

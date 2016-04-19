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
#ifndef XBUSMESSAGE_H
#define XBUSMESSAGE_H
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/*!	\struct XbusMessage
	\brief Xbus message
*/
struct XbusMessage
{
	uint8_t m_mid;

	uint16_t m_length;

	uint8_t* m_data;
	
#ifdef __cplusplus
	XbusMessage(uint8_t mid, uint16_t length = 0, uint8_t* data = NULL)
		: m_mid(mid)
		, m_length(length)
		, m_data(data)
	{}
#endif
};

/*!	\brief Low level bus format for transmitted Xbus messages
 */
enum XbusBusFormat
{
	/*! \brief Format for use with I2C interface. */
	XBF_I2c,
	/*! \brief Format for use with SPI interface. */
	XBF_Spi,
	/*! \brief Format for use with UART interface. */
	XBF_Uart
};

size_t XbusMessage_createRawMessage(uint8_t *raw, struct XbusMessage const *message, enum XbusBusFormat format);

#ifdef __cplusplus
}
//}
#endif // extern "C"


#endif // XBUSMESSAGE_H

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
#include "xbusmessage.h"
#include "xbusdef.h"

/*!	\brief Format a message into the raw mtssp format ready for transmission to a motion tracker.
*/
size_t XbusMessage_createRawMessage(uint8_t *raw, struct XbusMessage const *message, enum XbusBusFormat format)
{
	int n;
	uint8_t checksum;
	uint16_t length;
	uint8_t *dptr = raw;

	if (raw == 0)
	{
		switch (format)
		{
		case XBF_I2c:
			return (message->m_length < 255) ? message->m_length + 4 : message->m_length + 6;
		case XBF_Spi:
			return (message->m_length < 255) ? message->m_length + 7 : message->m_length + 9;
		case XBF_Uart:
			return (message->m_length < 255) ? message->m_length + 5 : message->m_length + 7;
		}
	}

	switch (format)
	{
	case XBF_I2c:
	{
		*dptr++ = XBUS_CONTROL_PIPE;
	}
		break;

	case XBF_Spi:
	{
		*dptr++ = XBUS_CONTROL_PIPE;
		// Fill bytes required to allow MT to process data
		*dptr++ = 0;
		*dptr++ = 0;
		*dptr++ = 0;
	}
		break;

	case XBF_Uart:
	{
		*dptr++ = XBUS_PREAMBLE;
		*dptr++ = XBUS_MASTERDEVICE;
	}
		break;
	}

	checksum = (uint8_t)(-XBUS_MASTERDEVICE);

	*dptr = message->m_mid;
	checksum -= *dptr++;

	length = message->m_length;

	if (length < XBUS_EXTENDED_LENGTH)
	{
		*dptr = length;
		checksum -= *dptr++;
	}
	else
	{
		*dptr = XBUS_EXTENDED_LENGTH;
		checksum -= *dptr++;
		*dptr = length >> 8;
		checksum -= *dptr++;
		*dptr = length & 0xFF;
		checksum -= *dptr++;
	}

	for (n = 0; n < message->m_length; n++)
	{
		*dptr = message->m_data[n];
		checksum -= *dptr++;
	}

	*dptr++ = checksum;

	return dptr - raw;
}

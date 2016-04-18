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
#include "xbusparser.h"
#include "xbusdef.h"
#include <stdlib.h>


/*! \brief XbusParser states. */
enum XbusParserState
{
	XBPS_Preamble,          /*!< \brief Looking for preamble. */
	XBPS_BusId,             /*!< \brief Waiting for bus ID. */
	XBPS_MessageId,         /*!< \brief Waiting for message ID. */
	XBPS_Length,            /*!< \brief Waiting for length. */
	XBPS_ExtendedLengthMsb, /*!< \brief Waiting for extended length MSB*/
	XBPS_ExtendedLengthLsb, /*!< \brief Waiting for extended length LSB*/
	XBPS_Payload,           /*!< \brief Reading payload. */
	XBPS_Checksum           /*!< \brief Waiting for checksum. */
};


/*!
 * \brief Xbus Parser state structure.
 */
struct XbusParser
{
	/*! \brief Callbacks for memory management, and message handling. */
	struct XbusParserCallback callbacks;
	/*! \brief Storage for the current message being received. */
	struct XbusMessage currentMessage;
	/*! \brief The number of bytes of payload received for the current message. */
	uint16_t payloadReceived;
	/*! \brief The calculated checksum for the current message. */
	uint8_t checksum;
	/*! \brief The state of the parser. */
	enum XbusParserState state;
};


/*!
 * \brief Get the amount of memory needed for the XbusParser structure.
 */
size_t XbusParser_mem(void)
{
	return sizeof(struct XbusParser);
}


/*!
 * \brief Create a new XbusParser object.
 * \param callback Pointer to callback structure containing callback functions
 * for memory management and handling received messages.
 * \returns Pointer the new XbusParser structure.
 *
 * Uses malloc to allocate the memory required for the parser.
 */
struct XbusParser* XbusParser_create(struct XbusParserCallback const* callback)
{
	void* mem = malloc(XbusParser_mem());
	if (mem)
	{
		return XbusParser_init(mem, callback);
	}
	return NULL;
}


/*!
 * \brief Frees an XbusParser structure allocated by XbusParser_create().
 */
void XbusParser_destroy(struct XbusParser* parser)
{
	free(parser);
}


/*!
 * \brief Initializes an XbusParser in the passed memory location.
 * \param parserMem Pointer to memory to use for storing parser state. Should
 * be at least as big as the value returned by XbusParser_mem().
 * \param callback Pointer to callback structure containing callback functions
 * for memory management and handling received messages.
 * \returns Initialized XbusParser structure.
 */
struct XbusParser* XbusParser_init(void* parserMem, struct XbusParserCallback const* callback)
{
	struct XbusParser* parser = (struct XbusParser*)parserMem;
	parser->state = XBPS_Preamble;
	parser->callbacks.allocateBuffer = callback->allocateBuffer;
	parser->callbacks.deallocateBuffer = callback->deallocateBuffer;
	parser->callbacks.handleMessage = callback->handleMessage;
	return parser;
}


/*!
 * \brief Prepare for receiving a message payload.
 *
 * Requests a memory area to store the received data to using the
 * registered callbacks.
 */
void prepareForPayload(struct XbusParser* parser)
{
	parser->payloadReceived = 0;
	parser->currentMessage.m_data = parser->callbacks.allocateBuffer(parser->currentMessage.m_length);
}


/*!
 * \brief Parse a byte of data from a motion tracker.
 *
 * When a complete message is received the user will be notified by a call
 * to the handleMessage() callback function.
 */
void XbusParser_parseByte(struct XbusParser *parser, const uint8_t byte)
{
	switch (parser->state)
	{
		case XBPS_Preamble:
			if (byte == XBUS_PREAMBLE)
			{
				parser->checksum = 0;
				parser->state = XBPS_BusId;
			}
			break;

		case XBPS_BusId:
			parser->checksum += byte;
			parser->state = XBPS_MessageId;
			break;

		case XBPS_MessageId:
			parser->checksum += byte;
			parser->currentMessage.m_mid = byte;
			parser->state = XBPS_Length;
			break;

		case XBPS_Length:
			parser->checksum += byte;
			if (byte == XBUS_NO_PAYLOAD)
			{
				parser->currentMessage.m_length = byte;
				parser->currentMessage.m_data = NULL;
				parser->state = XBPS_Checksum;
			}
			else if (byte < XBUS_EXTENDED_LENGTH)
			{
				parser->currentMessage.m_length = byte;
				prepareForPayload(parser);
				parser->state = XBPS_Payload;
			}
			else
			{
				parser->state = XBPS_ExtendedLengthMsb;
			}
			break;

		case XBPS_ExtendedLengthMsb:
			parser->checksum += byte;
			parser->currentMessage.m_length = ((uint16_t)byte) << 8;
			parser->state = XBPS_ExtendedLengthLsb;
			break;

		case XBPS_ExtendedLengthLsb:
			parser->checksum += byte;
			parser->currentMessage.m_length |= byte;
			prepareForPayload(parser);
			parser->state = XBPS_Payload;
			break;

		case XBPS_Payload:
			parser->checksum += byte;
			if (parser->currentMessage.m_data)
			{
				((uint8_t*)parser->currentMessage.m_data)[parser->payloadReceived] = byte;
			}
			if (++parser->payloadReceived == parser->currentMessage.m_length)
			{
				parser->state = XBPS_Checksum;
			}
			break;

		case XBPS_Checksum:
			parser->checksum += byte;
			if ((parser->checksum == 0) &&
					((parser->currentMessage.m_length == 0) ||
					 parser->currentMessage.m_data))
			{
				parser->callbacks.handleMessage(&parser->currentMessage);
			}
			else if (parser->currentMessage.m_data)
			{
				parser->callbacks.deallocateBuffer(parser->currentMessage.m_data);
			}
			parser->state = XBPS_Preamble;
			break;
	}
}


/*!
 * \brief Parse a buffer of data received from a motion tracker.
 */
void XbusParser_parseBuffer(struct XbusParser* parser, uint8_t const* buf, size_t bufSize)
{
	size_t i;
	for (i = 0; i < bufSize; ++i)
	{
		XbusParser_parseByte(parser, buf[i]);
	}
}



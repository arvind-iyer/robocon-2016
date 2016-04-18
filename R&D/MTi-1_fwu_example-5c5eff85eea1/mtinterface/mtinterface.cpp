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
#include "mtinterface.h"
#include <stdlib.h>
#include <assert.h>


#define XBUS_ERROR_MID	0x42

/*!	\class MtInterface
	\brief Abstract interface for communicating with an Xsens Motion Tracker (MT) on an mbed platform

	MtInterface defines an abstract interface for communicating with a Motion Tracker (MT) via xbus
	messages. Depending on the type of communication interface a specific implementation
	must be made which inherits from MtInterface.
*/

static MtInterface* g_thisPtr = 0;


/*!	\brief Callback function for XbusParserCallback for allocating a block of memory
	\param bufferSize buffer size
*/
static void *allocateMessageDataWrapper(size_t bufferSize)
{
	return g_thisPtr->allocateMessageData(bufferSize);
}


/*!	\brief Callback function for XbusParserCallback for deallocating a block of memory
	\param buffer Pointer to the buffer that should be deallocated
*/
static void deallocateMessageDataWrapper(void const *buffer)
{
	g_thisPtr->deallocateMessageData(buffer);
}


/*!	\brief Callback function for XbusParserCallback that handles completely received xbus messages from the motion tracker
	\param Pointer to the received xbus message
*/
static void xbusParserCallbackFunctionWrapper(struct XbusMessage const *message)
{
	g_thisPtr->xbusParserCallbackFunction(message);
}


/*!	\brief Constructs an MtInterface.
 *	\note Only a single instance of MtInterface is allowed
*/
MtInterface::MtInterface()
{
	assert(g_thisPtr == 0);
	g_thisPtr = this;

	// Create an xbusParser:
	XbusParserCallback xbusCallback = {};
	xbusCallback.allocateBuffer = allocateMessageDataWrapper;
	xbusCallback.deallocateBuffer = deallocateMessageDataWrapper;
	xbusCallback.handleMessage = xbusParserCallbackFunctionWrapper;
	m_xbusParser = XbusParser_create(&xbusCallback);
}


/*! \brief Destructor
*/
MtInterface::~MtInterface()
{
	g_thisPtr = 0;
}


/*! \brief Returns the next message from the rx queue or NULL if the queue is empty
	\note The caller must dealocate the message with releaseXbusMessage() after use.
*/
XbusMessage *MtInterface::getXbusMessage()
{
	XbusMessage *xbusMessage = NULL;
	osEvent ev = m_xbusRxQueue.get(1);
	if (ev.status == osEventMessage)
	{
		xbusMessage = (XbusMessage*)ev.value.p;
	}
	return xbusMessage;
}


/*!	\brief Releases an xbus message previously obtained by a call to getXbusMessage()
	\param xbusMessage that should be released
*/
void MtInterface::releaseXbusMessage(XbusMessage *xbusMessage)
{
	if (xbusMessage != NULL)
	{
		deallocateMessageData(xbusMessage->m_data);
		m_xbusMessagePool.free(xbusMessage);
	}
}


/*!	\brief Callback function for XbusParserCallback to allocate a block of memory
	\param bufferSize buffer size
*/
void *MtInterface::allocateMessageData(size_t bufferSize)
{
	assert(bufferSize < m_rxBufferSize);
	void *ptr = m_memoryPool.alloc();
	assert(ptr);
	return ptr;
}


/*!	\brief Callback function for XbusParserCallback to deallocate a block of memory
	\param buffer Pointer to the buffer that should be deallocated
*/
void MtInterface::deallocateMessageData(void const *buffer)
{
	m_memoryPool.free((uint8_t(*)[m_rxBufferSize])buffer);
}


/*!	\brief Callback function for XbusParserCallback that handles a completely received xbus message from the motion tracker
	\param message Pointer to the received xbus message
*/
void MtInterface::xbusParserCallbackFunction(struct XbusMessage const *message)
{
	XbusMessage *xbusMessage = m_xbusMessagePool.alloc();
	assert(xbusMessage);
	memcpy(xbusMessage, message, sizeof(XbusMessage));
	m_xbusRxQueue.put(xbusMessage);
}

/*! \brief Sends an Xbus message and waits for the related acknowledge
	\returns 0 if no related message was received. Pointer to the message otherwise. This message can also be an error message.
*/
XbusMessage* MtInterface::sendAndWait(const XbusMessage* xbusMessage)
{
	sendXbusMessage(xbusMessage);
	Timer timer;
	timer.start();
	bool waiting = true;
	
	uint8_t expectedMid = xbusMessage->m_mid + 1;
	
	while(waiting && (timer.read() < 1.0))
	{
		process();
		XbusMessage* xbusMessage = getXbusMessage();
		if (xbusMessage)
		{
			if (xbusMessage->m_mid == expectedMid || xbusMessage->m_mid == XBUS_ERROR_MID)
			{
				return xbusMessage;
			}
			releaseXbusMessage(xbusMessage);
		}
	}
	
	return NULL;
}


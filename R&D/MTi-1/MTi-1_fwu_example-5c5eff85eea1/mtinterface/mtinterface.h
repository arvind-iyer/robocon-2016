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
#ifndef MTINTERFACE_H
#define MTINTERFACE_H

#include "mbed.h"
#include "rtos.h"
#include "xbusmessage.h"
#include "xbusparser.h"

class MtInterface
{
public:
	MtInterface();
	virtual ~MtInterface();

	/*!	\brief This function must be periodically called from the main loop. Classes which inherid
		from MtInterface can place functionality which must run periodically in this function.
	*/
	virtual void process() = 0;

	/*!	\brief Interface for sending xbus messages to the module
	*/
	virtual void sendXbusMessage(XbusMessage const* xbusMessage) = 0;

	/*!	\brief Interface for requesting the low level format used by an MtInterface instance
	*/
	virtual XbusBusFormat busFormat() = 0;

	XbusMessage* getXbusMessage();
	void releaseXbusMessage(XbusMessage *xbusMessage);

	XbusMessage* sendAndWait(XbusMessage const* xbusMessage);

protected:
	static const int m_rxBufferSize = 300;
	MemoryPool<uint8_t[m_rxBufferSize], 3> m_memoryPool;
	MemoryPool<XbusMessage, 6> m_xbusMessagePool;
	XbusParser *m_xbusParser;
	Queue<XbusMessage, 4> m_xbusRxQueue;

	void *allocateMessageData(size_t bufferSize);
	void deallocateMessageData(void const *buffer);
	void xbusParserCallbackFunction(struct XbusMessage const *message);

	friend void *allocateMessageDataWrapper(size_t bufferSize);
	friend void deallocateMessageDataWrapper(void const *buffer);
	friend void xbusParserCallbackFunctionWrapper(struct XbusMessage const *message);
};

#endif //MTINTERFACE_H

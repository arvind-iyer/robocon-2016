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
#include "fwupdate.h"
#include "string.h"

#ifndef LOG
static void defaultLogFunction(char const* format, ...) { }
#define LOG defaultLogFunction
#endif

#define XMID_FIRMWARE_UPDATE (0xF2)

#define FWUP_DEFAULT_SLICE_SIZE	(64)


/*!	\brief Firmware updater commands
*/
#define FWUP_READY			(unsigned char)0x52	//'R'
#define FWUP_OK				(unsigned char)0x53	//'S'
#define FWUP_CSERROR		(unsigned char)0x45	//'E'
#define FWUP_CRITICAL		(unsigned char)0x46	//'F'
#define FWUP_FINISHED		(unsigned char)0x46	//'F'
#define FWUP_PAGE			(unsigned char)0x50	//'P'
#define FWUP_HEADER			(unsigned char)0x53	//'S'
#define FWUP_PAGESLICE		(unsigned char)0x54 //'T'
#define FWUP_STARTED		(unsigned char)0x41	//'A'
#define FWUP_OTHER			(unsigned char)0x4F	//'O'


/*!	\brief Helper function for converting a firmware updater command to a string
*/
const char *commandToString(uint8_t command)
{
	switch (command)
	{
		case FWUP_READY:		return("FWUP_READY");
		case FWUP_HEADER:		return("FWUP_HEADER");
		case FWUP_FINISHED:		return("FWUP_FINISHED");
		case FWUP_PAGE:			return("FWUP_PAGE");
		case FWUP_PAGESLICE:	return("FWUP_PAGESLICE");
		case FWUP_STARTED:		return("FWUP_STARTED");
		case FWUP_OTHER:		return("FWUP_OTHER");
		default:				return("unknown");
	}
}


/*!	\brief Helper function for converting a firmware updater command acknowledge to a string
*/
const char *ackToString(uint8_t command)
{
	switch (command)
	{
		case FWUP_READY:		return "FWUP_READY";
		case FWUP_OK:			return "FWUP_OK";
		case FWUP_CSERROR:		return "FWUP_CSERROR";
		case FWUP_CRITICAL:		return "FWUP_CRITICAL";
		case FWUP_PAGE:			return "FWUP_PAGE";
		case FWUP_PAGESLICE:	return "FWUP_PAGESLICE";
		case FWUP_STARTED:		return "FWUP_STARTED";
		case FWUP_OTHER:		return "FWUP_OTHER";
		default:				return "unknown";
	}
}


/*!	\brief Read a uint32_t from the current position in the xff
*/
uint32_t readUint32(FwUpdate *thisPtr)
{
	uint32_t result;
	uint8_t buffer[4];
	int n = thisPtr->m_readXffData(buffer, thisPtr->m_readIndex, 4);
	thisPtr->m_readIndex += n;
	if (n == 4)
	{
		result = buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3];
	}
	else
	{
		thisPtr->m_endOfFile = 1;
		result = 0;
	}
	return result;
}


/*!	\brief Read a uint16_t from the current position in the xff
*/
uint16_t readUint16(FwUpdate *thisPtr)
{
	uint16_t result;
	uint8_t buffer[2];
	int n = thisPtr->m_readXffData(buffer, thisPtr->m_readIndex, 2);
	thisPtr->m_readIndex += n;
	if (n == 2)
	{
		result = buffer[0] << 8 | buffer[1];
	}
	else
	{
		thisPtr->m_endOfFile = 1;
		result = 0;
	}
	return result;
}


/*!	\brief Read a uint8_t from the current position in the xff
*/
uint8_t readUint8(FwUpdate *thisPtr)
{
	uint8_t result;
	uint8_t buffer[1];
	int n = thisPtr->m_readXffData(buffer, thisPtr->m_readIndex, 1);
	thisPtr->m_readIndex += n;
	if (n == 1)
	{
		result = buffer[0];
	}
	else
	{
		thisPtr->m_endOfFile = 1;
		result = 0;
	}
	return result;
}


/*!	\brief Read an Xff header from the current position in the xff
*/
static void readXffHeader(FwUpdate *thisPtr)
{
	LOG("Fwu: readXffHeader()\n");

	thisPtr->m_xffHeader.m_globalId = readUint32(thisPtr);
	thisPtr->m_xffHeader.m_sectionSize = readUint32(thisPtr);
	thisPtr->m_xffHeader.m_firmwareRevision[0] = readUint8(thisPtr);
	thisPtr->m_xffHeader.m_firmwareRevision[1] = readUint8(thisPtr);
	thisPtr->m_xffHeader.m_firmwareRevision[2] = readUint8(thisPtr);
	thisPtr->m_xffHeader.m_xffVersion[0] = readUint8(thisPtr);
	thisPtr->m_xffHeader.m_xffVersion[1] = readUint8(thisPtr);
	thisPtr->m_xffHeader.m_chipId = readUint8(thisPtr);
	thisPtr->m_xffHeader.m_numberOfSections = readUint8(thisPtr);
	thisPtr->m_xffHeader.m_addressLength = readUint8(thisPtr);
	thisPtr->m_xffHeader.m_pageSize = readUint16(thisPtr);
	if (thisPtr->m_xffHeader.m_xffVersion[0] >= 2)
		thisPtr->m_xffHeader.m_sliceSize = readUint16(thisPtr);
	else
		thisPtr->m_xffHeader.m_sliceSize = FWUP_DEFAULT_SLICE_SIZE;
}


/*!	\brief Send an XMID_FIRMWARE_UPDATE xbus message
	\param data Pointer to the xbus message payload data
	\param length Number of payload bytes
*/
static void sendFirmwareUpdateCommand(FwUpdate *thisPtr, uint8_t *data, uint16_t length)
{
	struct XbusMessage xbusMessage;
	xbusMessage.m_mid = XMID_FIRMWARE_UPDATE;
	xbusMessage.m_length = length;
	xbusMessage.m_data = data;
	thisPtr->m_sendXbusMessage(&xbusMessage);
}


/*!	\brief Send a FWUP_READY command
*/
static void sendReady(FwUpdate *thisPtr)
{
	thisPtr->m_txBuffer[0] = FWUP_READY;
	sendFirmwareUpdateCommand(thisPtr, thisPtr->m_txBuffer, 1);
}


/*!	\brief Send a FWUP_HEADER command
*/
static void sendHeader(FwUpdate *thisPtr)
{
	int n;
	thisPtr->m_txBuffer[0] = FWUP_HEADER;
	n = thisPtr->m_readXffData(&thisPtr->m_txBuffer[1], thisPtr->m_readIndex, thisPtr->m_xffHeader.m_addressLength);
	thisPtr->m_readIndex += n;
	if (n == thisPtr->m_xffHeader.m_addressLength)
	{
		memcpy(&thisPtr->m_txBuffer[1 + thisPtr->m_xffHeader.m_addressLength], &thisPtr->m_nofSlicesPerPage, 2);
		sendFirmwareUpdateCommand(thisPtr, thisPtr->m_txBuffer, 1 + thisPtr->m_xffHeader.m_addressLength + 2);
	}
	else
	{
		thisPtr->m_endOfFile = 1;
	}
}


/*!	\brief Send a page slice
*/
static void sendSlice(FwUpdate *thisPtr)
{
	int n;
	thisPtr->m_txBuffer[0] = FWUP_PAGESLICE;
	n = thisPtr->m_readXffData(&thisPtr->m_txBuffer[1], thisPtr->m_readIndex, thisPtr->m_xffHeader.m_sliceSize);
	thisPtr->m_readIndex += n;
	if (n == thisPtr->m_xffHeader.m_sliceSize)
	{
		sendFirmwareUpdateCommand(thisPtr, thisPtr->m_txBuffer, 1 + thisPtr->m_xffHeader.m_sliceSize);
	}
	else
	{
		thisPtr->m_endOfFile = 1;
	}
}


/*!	\brief Send a FWUP_OTHER command
*/
static void sendOther(FwUpdate *thisPtr)
{
	thisPtr->m_txBuffer[0] = FWUP_OTHER;
	thisPtr->m_txBuffer[1] = thisPtr->m_xffHeader.m_chipId;
	LOG("Fwu: Send FWUP_OTHER\n");
	sendFirmwareUpdateCommand(thisPtr, thisPtr->m_txBuffer, 2);
}


/*!	\brief Send a FWUP_FINISHED command
*/
static void sendFinished(FwUpdate *thisPtr)
{
	thisPtr->m_txBuffer[0] = FWUP_FINISHED;
	LOG("Fwu: Send FWUP_FINISHED\n");
	sendFirmwareUpdateCommand(thisPtr, thisPtr->m_txBuffer, 1);
}


/*!	\brief Enter the next section of the xff file
*/
static void enterNewSection(FwUpdate *thisPtr)
{
	LOG("\nFwu: enterNewSection()\n");
	readXffHeader(thisPtr);
	thisPtr->m_nofPages = thisPtr->m_xffHeader.m_sectionSize / (thisPtr->m_xffHeader.m_pageSize + thisPtr->m_xffHeader.m_addressLength);
	thisPtr->m_nofSlicesPerPage = thisPtr->m_xffHeader.m_pageSize / thisPtr->m_xffHeader.m_sliceSize;
	thisPtr->m_pageCounter = 0;
	thisPtr->m_sliceCounter = 0;
	sendOther(thisPtr);
}


/*	\brief Public interface of the firmware updater:
*/

/*!	\brief Initialize a FwUpdate instance
*/
void FwUpdate_init(FwUpdate *thisPtr)
{
	LOG("Fwu: init()\n");
	thisPtr->m_state = STATE_Idle;
}


/*!	\brief Start a firmware update
*/
void FwUpdate_start(FwUpdate *thisPtr)
{
	if (thisPtr->m_state == STATE_Idle)
	{
		LOG("Fwu: start() --> Send FWUP_READY\n");
		thisPtr->m_readIndex = 0;
		thisPtr->m_endOfFile = 0;
		thisPtr->m_state = STATE_Start;
		sendReady(thisPtr);
	}
	else
	{
		thisPtr->m_readyHandler(FWU_Failed);
		LOG("Fwu: start() failed\n");
	}
}


/*!	\brief Handle xbus message coming from the module
	\param xbusMessage The xbus message from the module to be handled
*/
void FwUpdate_handleXbus(FwUpdate* thisPtr, struct XbusMessage* xbusMessage)
{
	uint8_t ack;
	if (xbusMessage->m_mid != XMID_FIRMWARE_UPDATE)
	{
		LOG("Fwu: Got unhandled xbus message 0x%02X (ignored)\n", xbusMessage->m_mid);
		return;
	}

	ack = xbusMessage->m_data[0];

	switch (thisPtr->m_state)
	{
		case STATE_Idle:
		{
			LOG("Fwu: Got %s in STATE_Idle (ignored)\n", ackToString(ack));
		} break;

		case STATE_Start:
		{
			if (ack == FWUP_READY)
			{
				LOG("Fwu: FWUP_READY in STATE_Start --> Enter new section\n");
				enterNewSection(thisPtr);
				thisPtr->m_state = STATE_WaitReady;
			}
			else
			{
				LOG("Fwu: Got %s in STATE_Start (ignored)\n", ackToString(ack));
			}
		} break;

		case STATE_WaitReady:
		{
			if (ack == FWUP_READY)
			{
				LOG("Fwu: FWUP_READY in STATE_WaitReady --> Send header\n");
				sendHeader(thisPtr);
				thisPtr->m_state = STATE_WaitHeaderResult;
			}
			else
			{
				LOG("Fwu: Got %s in STATE_WaitReady --> Failed\n", ackToString(ack));
				thisPtr->m_readyHandler(FWU_Failed);
				thisPtr->m_state = STATE_Idle;
			}
		} break;

		case STATE_WaitHeaderResult:
		{
			if (ack == FWUP_READY)
			{
				LOG("Fwu: FWUP_READY in STATE_WaitHeaderResult --> Send first slice\n");
				sendSlice(thisPtr);
				thisPtr->m_sliceCounter = 1;
				thisPtr->m_state = STATE_WaitSliceReady;
			}
			else
			{
				LOG("Fwu: Got %s in STATE_WaitHeaderResult --> Failed\n", ackToString(ack));
				thisPtr->m_readyHandler(FWU_Failed);
				thisPtr->m_state = STATE_Idle;
			}
		} break;

		case STATE_WaitSliceReady:
		{
			if (ack == FWUP_READY)
			{
				if (thisPtr->m_sliceCounter < thisPtr->m_nofSlicesPerPage)
				{
					LOG("Fwu: FWUP_READY in STATE_WaitSliceReady --> Send slice %d\n", thisPtr->m_sliceCounter);
					sendSlice(thisPtr);
					thisPtr->m_sliceCounter++;
				}
				else
				{
					LOG("Fwu: All slices sent --> STATE_WaitPageOk\n");
					thisPtr->m_state = STATE_WaitPageOk;
				}
			}
			else
			{
				LOG("Fwu: Got %s in STATE_WaitSliceReady --> Failed\n", ackToString(ack));
				thisPtr->m_readyHandler(FWU_Failed);
				thisPtr->m_state = STATE_Idle;
			}
		} break;

		case STATE_WaitPageOk:
		{
			if (ack == FWUP_OK)
			{
				LOG("Fwu: FWUP_OK in STATE_WaitPageOk --> STATE_WaitPageReady\n");
				thisPtr->m_state = STATE_WaitPageReady;
			}
			else
			{
				LOG("Fwu: Got %s in STATE_WaitPageOk --> Failed\n", ackToString(ack));
				thisPtr->m_readyHandler(FWU_Failed);
				thisPtr->m_state = STATE_Idle;
			}
		} break;

		case STATE_WaitPageReady:
		{
			if (ack == FWUP_READY)
			{
				thisPtr->m_pageCounter++;
				if (thisPtr->m_nofPages != 0 && thisPtr->m_pageCounter == thisPtr->m_nofPages)
				{
					LOG("Fwu: All pages sent --> Enter new section\n");
					enterNewSection(thisPtr);
					thisPtr->m_state = STATE_WaitReady;
				}
				else
				{
					sendHeader(thisPtr);
					if (thisPtr->m_endOfFile)
					{
						LOG("Fwu: End of file --> Firmware update done\n");
						sendFinished(thisPtr);
						thisPtr->m_readyHandler(FWU_Success);
						thisPtr->m_state = STATE_Idle;
					}
					else
					{
						LOG("Fwu: FWUP_READY in STATE_WaitPageReady --> Send header\n");
						thisPtr->m_state = STATE_WaitHeaderResult;
					}
				}
			}
			else
			{
				LOG("Fwu: Got %s in STATE_WaitPageReady --> Failed\n", ackToString(ack));
				thisPtr->m_readyHandler(FWU_Failed);
				thisPtr->m_state = STATE_Idle;
			}
		} break;

		default:
		{
		}
	}
}



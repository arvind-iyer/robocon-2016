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
#include "mtinterface_mtssp.h"
#include "mbed.h"
#include "rtos.h"
#include "board.h"
#include "xbusmessage.h"
#include "xbusparser.h"
#include "assert.h"
#include "xbusdef.h"


/*!	\class MtInterfaceMtssp
	\brief Implementation of MtInterface for the MTSSP protocol

	MTSSP is the protocol used for communicating with an Xsens motion tracker over I2C or SPI.
*/


/*!	\brief Constructor
	\param driver Pointer to an MtsspDriver
*/
MtInterfaceMtssp::MtInterfaceMtssp(MtsspDriver *driver)
	: m_driver(driver)
{
	m_dataReady = new DigitalIn(MT_DRDY);
}


/*!	\brief Destructor
*/
MtInterfaceMtssp::~MtInterfaceMtssp()
{
	delete m_dataReady;
}


/*!	\brief Must be polled in the main application loop
*/
void MtInterfaceMtssp::process()
{
	if (*m_dataReady)
	{
		handleDataReady();
	}
}


/*! \brief Sends an xbus message to the motion tracker
	\param xbusMessage Pointer to xbus message which should be send
*/
void MtInterfaceMtssp::sendXbusMessage(XbusMessage const* xbusMessage)
{
	uint8_t* buf = (uint8_t*)allocateMessageData(xbusMessage->m_length + 4);
	size_t rawLength = XbusMessage_createRawMessage(buf, xbusMessage, m_driver->busFormat());
	m_driver->writeRaw(buf, rawLength);
	deallocateMessageData(buf);
}


/*! \brief Returns the low level bus format used
*/
XbusBusFormat MtInterfaceMtssp::busFormat()
{
	return m_driver->busFormat();
}


/*! \brief Should be called if the data ready line from the motion tracker signals that there is data pending
*/
void MtInterfaceMtssp::handleDataReady()
{
	uint16_t notificationMessageSize = 0;
	uint16_t measurementMessageSize = 0;
	readPipeStatus(&notificationMessageSize, &measurementMessageSize);

	uint16_t size;
	uint8_t pipe;
	if (notificationMessageSize)
	{
		size = notificationMessageSize;
		pipe = XBUS_NOTIFICATION_PIPE;
	}
	else if (measurementMessageSize)
	{
		size = measurementMessageSize;
		pipe = XBUS_MEASUREMENT_PIPE;
	}
	else
	{
		return;
	}

	uint8_t* buffer = (uint8_t*)allocateMessageData(size + 3);
	buffer[0] = XBUS_PREAMBLE;
	buffer[1] = XBUS_MASTERDEVICE;
	readFromPipe(&buffer[2], size, pipe);
	XbusParser_parseBuffer(m_xbusParser, buffer, 2 + size);
	deallocateMessageData(buffer);
}


/*!	\brief Read MTSSP protocol info
	\param[out] version Pointer to receive the version byte
	\param[out] dataReadyConfig Pointer to receive the data ready configuration byte
	\sa configureProtocol
*/
void MtInterfaceMtssp::readProtocolInfo(uint8_t* version, uint8_t* dataReadyConfig)
{
	uint8_t rxdata[2];
	m_driver->read(XBUS_PROTOCOL_INFO, rxdata, sizeof(rxdata));
	*version = rxdata[0];
	*dataReadyConfig = rxdata[1];
}


/*!	\brief Write MTSSP protocol settings
	\param dataReadyConfig The data ready configuration which must be set

	Bit 7:4	Reserved \n
	Bit 3	Measurement pipe DRDY event enable: 0 = disabled, 1 = enabled \n
	Bit 2	Notification pipe DRDY event enable: 0 = disabled, 1 = enabled \n
	Bit 1	Output type of DRDY pin: = 0 Push/pull, 1 = open drain \n
	Bit 0	Polarity of DRDY signal: 0 = Idle low, 1 = Idle high \n
	\sa readProtocolInfo
*/
void MtInterfaceMtssp::configureProtocol(uint8_t dataReadyConfig)
{
	m_driver->write(XBUS_CONFIGURE_PROTOCOL, &dataReadyConfig, sizeof(dataReadyConfig));
}


/*!	\brief Read the pipe status
	\param[out] notificationMessageSize Pointer for returning the number of pending notification bytes
	\param[out] measurementMessageSize Pointer for returning the number of pending measurement bytes
*/
void MtInterfaceMtssp::readPipeStatus(uint16_t *notificationMessageSize, uint16_t* measurementMessageSize)
{
	uint8_t status[4];
	m_driver->read(XBUS_PIPE_STATUS, status, sizeof(status));
	*notificationMessageSize = status[0] | (status[1] << 8);
	*measurementMessageSize = status[2] | (status[3] << 8);
}


/*!	\brief Read from notification or measurement data pipe
	\param buffer Result buffer
	\param size Number of bytes to read
	\param pipe Pipe from which to read, XBUS_NOTIFICATION_PIPE or XBUS_MEASUREMENT_PIPE
*/
void MtInterfaceMtssp::readFromPipe(uint8_t* buffer, uint16_t size, uint8_t pipe)
{
	m_driver->read(pipe, buffer, size);
}

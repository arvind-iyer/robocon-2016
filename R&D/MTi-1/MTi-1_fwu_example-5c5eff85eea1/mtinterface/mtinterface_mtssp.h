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
#ifndef MTINTERFACE_MTSSP_H
#define MTINTERFACE_MTSSP_H

#include "xbusmessage.h"
#include "xbusparser.h"
#include "mtinterface.h"
#include "mbed.h"


/*!	\class MtsspDriver
	\brief Abstract interface providing the MTSSP interface with a abstraction layer to the underlying hardware bus

*/
class MtsspDriver
{
public:

	/*!	\brief Perform a blocking write transfer
		\param opcode Opcode to use
		\param data Pointer to data to be written
		\param dataLength Number of data bytes to write
	*/
	virtual void write(uint8_t opcode, uint8_t const* data, int dataLength) = 0;


	/*!	\brief Perform a blocking read transfer
		\param opcode Opcode to use
		\param data Pointer to result buffer
		\param dataLength Number of data bytes to read
	*/
	virtual void read(uint8_t opcode, uint8_t* data, int dataLength) = 0;


	/*!	\brief Perform a blocking write transfer
		\param data Pointer to data to be written
		\param dataLength Number of data bytes to write
	*/
	virtual void writeRaw(uint8_t const* data, int dataLength) = 0;


	/*!	\brief Returns the low level bus format that must be used for tranmitting messages over this hardware bus
	*/
	virtual XbusBusFormat busFormat() const = 0;
};



class MtInterfaceMtssp : public MtInterface
{
public:
	MtInterfaceMtssp(MtsspDriver* driver);
	virtual ~MtInterfaceMtssp();

	virtual void process();
	virtual void sendXbusMessage(XbusMessage const* xbusMessage);
	virtual XbusBusFormat busFormat();

protected:
	void handleDataReady();
	void readProtocolInfo(uint8_t* version, uint8_t* dataReadyConfig);
	void configureProtocol(uint8_t dataReadyConfig);
	void readPipeStatus(uint16_t* notificationMessageSize, uint16_t* measurementMessageSize);
	void readFromPipe(uint8_t* buffer, uint16_t size, uint8_t pipe);

private:
	MtsspDriver* m_driver;
	DigitalIn* m_dataReady;
};


#endif // MTINTERFACE_MTSSP_H

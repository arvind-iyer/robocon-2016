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
#include "mtssp_i2c_driver.h"
#include <string.h>
#include <assert.h>
#include "board.h"

#define MTI_I2C_ADDRESS 			(0x1D << 1)
#define MTI_I2C_ADDRESS_SELECTOR	0


/*!	\class MtsspI2cDriver
	\brief Implementation of MtsspDriver for communicating over I2C.
	
	The low level I2C driver is provided by the mbed platform.
*/


/*!	\brief Constructor
*/
MtsspI2cDriver::MtsspI2cDriver()
{
	m_slaveAddress = new BusOut(MT_ADD0, MT_ADD1, MT_ADD2);
	m_slaveAddress->write(MTI_I2C_ADDRESS_SELECTOR);
	m_i2c = new I2C(MT_SDA, MT_SCL);
	m_i2c->frequency(400000);
}


/*!	\brief Destructor
*/
MtsspI2cDriver::~MtsspI2cDriver()
{
	delete m_i2c;
}


/*!	\brief Perform a blocking write transfer over I2C
	\param opcode Opcode to use
	\param data Pointer to data to be written
	\param dataLength Number of data bytes to write
*/
void MtsspI2cDriver::write(uint8_t opcode, uint8_t const* data, int dataLength)
{
	uint8_t transferBuffer[8];
	assert(dataLength < sizeof(transferBuffer));
	transferBuffer[0] = opcode;
	memcpy(&transferBuffer[1], data, dataLength);
	m_i2c->write(MTI_I2C_ADDRESS, (char*)transferBuffer, dataLength + 1);
}


/*!	\brief Perform a blocking read transfer over I2C
	\param opcode Opcode to use
	\param data Pointer to result buffer
	\param dataLength Number of data bytes to read
*/
void MtsspI2cDriver::read(uint8_t opcode, uint8_t* data, int dataLength)
{
	m_i2c->write(MTI_I2C_ADDRESS, (char*)&opcode, sizeof(opcode), true);
	m_i2c->read(MTI_I2C_ADDRESS, (char*)data, dataLength);
}


/*!	\brief Perform a blocking write transfer over I2C
	\param data Pointer to data to be written
	\param dataLength Number of data bytes to write
*/
void MtsspI2cDriver::writeRaw(uint8_t const* data, int dataLength)
{
	m_i2c->write(MTI_I2C_ADDRESS, (char*)data, dataLength);
}

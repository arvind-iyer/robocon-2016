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
#include "mtssp_spi_driver.h"
#include <string.h>
#include "board.h"


/*!	\class MtsspSpiDriver
	\brief Implementation of MtsspDriver for communicating over SPI.
	
	The low level SPI driver is provided by the mbed platform.
*/


/*!	\brief Constructor
*/
MtsspSpiDriver::MtsspSpiDriver()
{
	m_chipSelect = new DigitalOut(MT_nCS, 1);
	*m_chipSelect = 1;
	wait(0.1);
	m_spi = new SPI(MT_MOSI, MT_MISO, MT_SCLK);
	
	m_spi->frequency(1000000);
	m_spi->format(8, 3);
}


/*!	\brief Destructor
*/
MtsspSpiDriver::~MtsspSpiDriver()
{
	delete m_chipSelect;
	delete m_spi;
}


/*!	\brief Perform a blocking write transfer over SPI
	\param opcode Opcode to use
	\param data Pointer to data to be written
	\param dataLength Number of data bytes to write
*/
void MtsspSpiDriver::write(uint8_t opcode, uint8_t const* data, int dataLength)
{
	*m_chipSelect = 0;
	m_spi->write(opcode);
	m_spi->write(0);
	m_spi->write(0);
	m_spi->write(0);

	for (int i = 0; i < dataLength; i++)
	{
		m_spi->write(data[i]);
	}

	*m_chipSelect = 1;
}


/*!	\brief Perform a blocking read transfer over SPI
	\param opcode Opcode to use
	\param data Pointer to result buffer
	\param dataLength Number of data bytes to read
*/
void MtsspSpiDriver::read(uint8_t opcode, uint8_t* data, int dataLength)
{
	*m_chipSelect = 0;

	m_spi->write(opcode);
	m_spi->write(0);
	m_spi->write(0);
	m_spi->write(0);

	for (int i = 0; i < dataLength; i++)
	{
		data[i] = m_spi->write(0);
	}
	*m_chipSelect = 1;
}


/*!	\brief Perform a blocking write transfer over SPI
	\param data Pointer to data to be written
	\param dataLength Number of data bytes to write
*/
void MtsspSpiDriver::writeRaw(uint8_t const* data, int dataLength)
{
	*m_chipSelect = 0;
	for (int i = 0; i < dataLength; i++)
	{
		m_spi->write(data[i]);
	}
	*m_chipSelect = 1;
}

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
#ifndef MTSSP_SPI_H
#define MTSSP_SPI_H

#include "mtinterface_mtssp.h"

class MtsspSpiDriver : public MtsspDriver
{
public:
	MtsspSpiDriver();
	virtual ~MtsspSpiDriver();

	virtual void write(uint8_t opcode, uint8_t const* data, int dataLength);
	virtual void read(uint8_t opcode, uint8_t* dest, int destMax);
	virtual void writeRaw(uint8_t const* data, int dataLength);

	virtual XbusBusFormat busFormat() const { return XBF_Spi; }

private:
	SPI* m_spi;
	DigitalOut *m_chipSelect;
};


#endif //MTSSP_SPI_H

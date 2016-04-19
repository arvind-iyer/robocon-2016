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
#ifndef MTINTERFACE_UART_H
#define MTINTERFACE_UART_H
#include "mbed.h"
#include "mtinterface.h"


class MtInterfaceUart : public MtInterface
{
	public:
		MtInterfaceUart(int baudrate);
		virtual ~MtInterfaceUart();

		virtual void process() {}
		virtual void sendXbusMessage(XbusMessage const* xbusMessage);
		virtual XbusBusFormat busFormat();

		void setBaudrate(int baudrate);

	private:
		static void uart_rx_interrupt_handler();

		Serial* m_serial;
		uint8_t* m_txBuffer;
};



#endif // MTINTERFACE_UART_H

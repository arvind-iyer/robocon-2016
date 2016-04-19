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
#include "mtinterface_uart.h"
#include "assert.h"
#include "board.h"
#include "mbed.h"


/*!	\class MtInterfaceUart
	\brief Implementation of MtInterface for communicating over UART.

	The low level UART driver is provided by the mbed platform.
*/

static MtInterfaceUart* thisPtr = 0;	// required to give callback function access to class members


/*!	\brief Constructor
	\param baudrate Uart baud rate used to communicate with the module
*/
MtInterfaceUart::MtInterfaceUart(int baudrate)
{
	assert(thisPtr == 0);	//(only one instance is allowed)
	thisPtr = this;
	m_serial = new Serial(UART_TX_TO_MODULE, UART_RX_FROM_MODULE);
	m_serial->baud(baudrate);
	m_serial->format(8, Serial::None, 1);
	m_serial->attach(uart_rx_interrupt_handler, Serial::RxIrq);
	m_txBuffer = new uint8_t[300];
}


/*!	\brief Destructor
*/
MtInterfaceUart::~MtInterfaceUart()
{
	delete m_serial;
	delete[] m_txBuffer;
	thisPtr = 0;
}


/*!	\brief Sends an xbus message to the module via UART
	\param xbusMessage Xbus message to be send
*/
void MtInterfaceUart::sendXbusMessage(XbusMessage const* xbusMessage)
{
	int size = XbusMessage_createRawMessage(m_txBuffer, xbusMessage, XBF_Uart);
	for (int n = 0; n < size; n++)
	{
		m_serial->putc(m_txBuffer[n]);
	}
}


/*!	\brief Returns the low level bus format used by this interface
*/
XbusBusFormat MtInterfaceUart::busFormat()
{
	return XBF_Uart;
}


/*!	\brief Handles UART bytes received from the motion tracker
*/
void MtInterfaceUart::uart_rx_interrupt_handler()
{
	while (thisPtr->m_serial->readable())
	{
		uint8_t c = thisPtr->m_serial->getc();
		XbusParser_parseBuffer(thisPtr->m_xbusParser, &c, 1);
	}
}


/*!	\brief Set the baudrate
	\param baudrate The baudrate value which should be set
*/
void MtInterfaceUart::setBaudrate(int baudrate)
{
	m_serial->baud(baudrate);
}


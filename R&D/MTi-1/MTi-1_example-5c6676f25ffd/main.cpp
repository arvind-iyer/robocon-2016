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
 *
 * \page Overview Firmware overview
 *
 * Example firmware for communicating with an Xsens MTi-1 series motion
 * tracker (MT).
 *
 * The firmware uses the mbed-rtos library to provide RTOS features such as
 * memory pools and queues. A single thread (main) is used with reception of
 * data from the motion tracker.
 *
 * \section Hardware setup
 * The firmware has been tested with a ST Nucleo F302R8 development board.
 * The Nucleo board should be connected to the MTi1 development board using the
 * Arduino compatible headers on the Nucleo board as follows:
 *
 * | Nucleo pin | MTi1 func.  | MTi1 dev. pin | Used for PSEL |
 * |------------|-------------|---------------|---------------|
 * | 5V         | VDD         | P300-1        | Any           |
 * | IORef      | VDDIO       | P300-2        | Any           |
 * | GND        | GND         | P300-3        | Any           |
 * | D2         | nRST        | P300-5        | Any           |
 * | SCL/D15    | UART_TX/SCL | P300-9        | UART / I2C    |
 * | SDA/D14    | UART_RX/SDA | P300-11       | UART / I2C    |
 * | D3         | DRDY        | P300-15       | SPI / I2C     |
 * | SCK/D13    | SCK/ADD0    | P300-17       | SPI / I2C     |
 * | MISO/D12   | MISO/ADD1   | P300-19       | SPI / I2C     |
 * | MOSI/D11   | MOSI/ADD2   | P300-21       | SPI / I2C     |
 * | CS/D10     | nCS         | P300-23       | SPI           |
 *
 * Communication with the host PC is achieved using the built-in USB serial
 * bridge of the Nucleo board. Communication with the MT is achieved through
 * either the UART, I2C or SPI interface. The active interface is chosen
 * on the MT's side by use of the PSEL0 and PSEL1 switch on the MTi1
 * development board. This example needs to be built with the matching
 * MTI_USES_xxxx_INTERFACE define set (see below)
 *
 * \subsection Porting
 * To port to a different mbed platform the following pin definitions need
 * to be updated.
 * In all cases: the reset line pin
 * For UART: the serial Rx/Tx lines UART_TX and UART_RX
 * For I2C: the SCL,SDA,DRDY and address lines
 * For SPI: The SCK,MISO,MOSI,nCS and DRDY lines
 *
 * \section Firmware Operation
 * The firmware starts by initializing the serial ports used to communicate
 * with the host PC and with the MT. During the initialization the MT is held
 * in reset using the nRST input.
 *
 * Once the firmware is ready to communicate with the MT the reset line is
 * released and the firmware waits for a wakeup message from the MT. If this is
 * not received within 1 second the firmware will try to restore communication
 * with the MT using a special restore communication procedure.
 *
 * When the MT is ready for communication the firmware requests the device ID
 * of the MT, and based on this determines which type of MTi is connected.
 * If the MT is an MTi-1 then it will be configured to send inertial and
 * magnetic measurement data. MTi-2 and MTi-3 devices have onboard orientation
 * estimation and will therefore be configured to provide quaternion output.
 */

#include "mbed.h"
#include "rtos.h"
#include "xbusparser.h"
#include "xbusmessage.h"
#include "xsdeviceid.h"
#include "xbusdef.h"

// Select communication interface to use for MTi
#define MTI_USES_I2C_INTERFACE

#if !(defined(MTI_USES_I2C_INTERFACE) || defined(MTI_USES_SPI_INTERFACE) || defined(MTI_USES_UART_INTERFACE))
#error "Must select communication interface by defining one of: MTI_USES_I2C_INTERFACE, MTI_USES_SPI_INTERFACE or MTI_USES_UART_INTERFACE"
#endif

#if defined(TARGET_NUCLEO_F302R8)

#define PC_TX 		PA_2
#define PC_RX 		PA_3
#define MT_TX 		PB_9
#define MT_RX 		PB_8
#define MT_SDA 		PB_9
#define MT_SCL 		PB_8
#define MT_ADD0 	PB_13
#define MT_ADD1 	PB_14
#define MT_ADD2 	PB_15
#define MT_MOSI 	PB_15
#define MT_MISO 	PB_14
#define MT_SCLK 	PB_13
#define MT_nCS 		PB_6
#define MT_NRESET 	PA_10
#define MT_DRDY 	PB_3

#elif defined(TARGET_KL46Z)

#define PC_TX 		USBTX
#define PC_RX 		USBRX
#define MT_TX 		PTE0
#define MT_RX 		PTE1
#define MT_SDA 		PTE0
#define MT_SCL 		PTE1
#define MT_ADD0 	PTD5
#define MT_ADD1 	PTD7
#define MT_ADD2 	PTD6
#define MT_MOSI 	PTD6
#define MT_MISO 	PTD7
#define MT_SCLK 	PTD5
#define MT_nCS 		PTD4
#define MT_NRESET 	PTD3
#define MT_DRDY 	PTD2

#elif defined(TARGET_LPC4088)

#define PC_TX 		USBTX
#define PC_RX 		USBRX
#define MT_TX 		p9
#define MT_RX 		p10
#define MT_SDA 		p9
#define MT_SCL 		p10
#define MT_ADD0 	p13
#define MT_ADD1 	p12
#define MT_ADD2 	p11
#define MT_MOSI 	p11
#define MT_MISO 	p12
#define MT_SCLK 	p13
#define MT_nCS 		p14
#define MT_NRESET 	p8
#define MT_DRDY 	p15

#else

#error "Support for selected mbed platform has not been added."

#endif


/*!
 * \brief Baudrate used to communicate with host PC.
 */
#define PC_UART_BAUDRATE (921600)

/*!
 * \brief The number of items to hold in the memory pools.
 */
#define MEMORY_POOL_SIZE (4)
/*!
 * \brief The size of the queue used for device responses.
 * This is set to one as in typical Xbus operation each command receives a
 * response before the next command is sent.
 */
#define RESPONSE_QUEUE_SIZE (1)
/*!
 * \brief The size of the queue used for data messages.
 * This is set to two to allow some overlap between printing received data to
 * the PC serial port and the reception of the subsequent data packet. In
 * more complex applications it might be necessary to increase this if
 * message processing might occasionally require more time than normal.
 */
#define DATA_QUEUE_SIZE (2)
/*!
 * \brief The maximum size of an xbus message supported by the application.
 * This is the size of the message buffers in the message data memory pool.
 */
#define MAX_XBUS_DATA_SIZE (128)

/*! \brief Serial port for communication with the host PC. */
static Serial pc(PC_TX, PC_RX);

#if defined(MTI_USES_I2C_INTERFACE)
/*!
 * \brief I2C master used for communication with the MT.
 */
static I2C mt(MT_SDA, MT_SCL);
static DigitalOut add0(MT_ADD0);
static DigitalOut add1(MT_ADD1);
static DigitalOut add2(MT_ADD2);

#elif defined(MTI_USES_SPI_INTERFACE)
/*! \brief SPI master used for communication with the MT. */
static SPI mt(MT_MOSI, MT_MISO, MT_SCLK);

/*! \brief Chip select line for the MT. */
static DigitalOut cs(MT_nCS, 1);

#elif defined(MTI_USES_UART_INTERFACE)
/*!
 * \brief Serial port for communication with the MT.
 *
 * We use a RawSerial port as the Stream inteface used by the regular
 * Serial class can have problems with the RTOS when using interrupts.
 */
static RawSerial mt(MT_TX, MT_RX);
#endif

#if defined(MTI_USES_I2C_INTERFACE) || defined(MTI_USES_SPI_INTERFACE)
/*!
 * \brief Interrput line used by MT to signal that data is available.
 */
static InterruptIn drdy(MT_DRDY);
#endif

/*!
 * \brief MT reset line.
 *
 * MT is held in reset on startup.
 */
static DigitalOut mtReset(MT_NRESET, 0);
/*! \brief XbusParser used to parse incoming Xbus messages from the MT. */
static XbusParser* xbusParser;

/*!
 * \brief Memory pool used for storing Xbus messages when passing them
 * to the main thread.
 */
MemoryPool<XbusMessage, MEMORY_POOL_SIZE> g_messagePool;
/*!
 * \brief Memory pool used for storing the payload of Xbus messages.
 */
MemoryPool<uint8_t[MAX_XBUS_DATA_SIZE], MEMORY_POOL_SIZE> g_messageDataPool;
/*!
 * \brief Queue used to pass data messages to the main thread for processing.
 */
Queue<XbusMessage, DATA_QUEUE_SIZE> g_dataQueue;
/*!
 * \brief Queue used for passing all other messages to the main thread for processing.
 */
Queue<XbusMessage, RESPONSE_QUEUE_SIZE> g_responseQueue;

/*!
 * \brief Allocate message data buffer from the message data pool.
 */
static void* allocateMessageData(size_t bufSize)
{
	return bufSize < MAX_XBUS_DATA_SIZE ? g_messageDataPool.alloc() : NULL;
}

/*!
 * \brief Deallocate message data previously allocated from the message
 * data pool.
 */
static void deallocateMessageData(void const* buffer)
{
	g_messageDataPool.free((uint8_t(*)[MAX_XBUS_DATA_SIZE])buffer);
}

#if defined(MTI_USES_I2C_INTERFACE)
#define MTI_I2C_ADDRESS (0x1D << 1)
static void readData(uint8_t pipe, uint16_t dataLength)
{
	const int preambleLength = 2;
	uint8_t* buf = (uint8_t*)allocateMessageData(dataLength+preambleLength);
	if (buf)
	{
		buf[0] = XBUS_PREAMBLE;
		buf[1] = XBUS_MASTERDEVICE;
		mt.write(MTI_I2C_ADDRESS, (char*)&pipe, sizeof(pipe), true);
		mt.read(MTI_I2C_ADDRESS, (char*)buf+preambleLength, dataLength);
		XbusParser_parseBuffer(xbusParser, buf, dataLength+preambleLength);
		deallocateMessageData(buf);
	}
}
static void mtInterruptHandler(void)
{
	while (true)
	{
		uint8_t opcode = XBUS_PIPE_STATUS;
		uint8_t status[4];
		mt.write(MTI_I2C_ADDRESS, (char*)&opcode, sizeof(opcode), true);
		mt.read(MTI_I2C_ADDRESS, (char*)status, sizeof(status));

		uint16_t notificationSize = status[0] | (status[1] << 8);
		uint16_t measurementSize = status[2] | (status[3] <<8);

		if (notificationSize)
		{
			readData(XBUS_NOTIFICATION_PIPE, notificationSize);
		}
		else if (measurementSize)
		{
			readData(XBUS_MEASUREMENT_PIPE, measurementSize);
		}
		else
			break; // No more data available to read.
	}
}

static void configureMtCommunicationInterface(void)
{
	mt.frequency(400000);
	//Use the addX pins to configure I2C address 0x1D
	add0.write(0);
	add1.write(0);
	add2.write(0);
	drdy.rise(&mtInterruptHandler);
}

/*!
 * \brief Send a message to the MT
 *
 * This function formats the message data and writes this to the MT I2C
 * interface. It does not wait for any response.
 */
static void sendMessage(XbusMessage const* m)
{
	uint8_t buf[64];
	size_t rawLength = XbusMessage_format(buf, m, XLLF_I2c);
	mt.write(MTI_I2C_ADDRESS, (char*)buf, rawLength);
}
#elif defined(MTI_USES_SPI_INTERFACE)
static void sendOpcode(uint8_t opcode)
{
	mt.write(opcode);
	for (int filler = 0; filler < 3; ++filler)
	{
		mt.write(filler);
	}
}

static void readData(uint8_t pipe, uint16_t dataLength)
{
	const int preambleLength = 2;
	uint8_t* buf = (uint8_t*)allocateMessageData(dataLength+preambleLength);
	if (buf)
	{
		uint8_t* dptr = buf;
		*dptr++ = XBUS_PREAMBLE;
		*dptr++ = XBUS_MASTERDEVICE;
		cs = 0;
		sendOpcode(pipe);
		for (int i = 0; i < dataLength; ++i)
		{
			*dptr++ = mt.write(0);
		}
		cs = 1;
		XbusParser_parseBuffer(xbusParser, buf, dptr - buf);
		deallocateMessageData(buf);
	}
}
static void mtInterruptHandler(void)
{
	while (true)
	{
		cs = 0;
		sendOpcode(XBUS_PIPE_STATUS);
		uint8_t status[4];
		for (int i = 0; i < sizeof(status); ++i)
		{
			status[i] = mt.write(0);
		}
		cs = 1;

		uint16_t notificationSize = status[0] | (status[1] << 8);
		uint16_t measurementSize = status[2] | (status[3] <<8);

		if (notificationSize)
		{
			readData(XBUS_NOTIFICATION_PIPE, notificationSize);
		}
		else if (measurementSize)
		{
			readData(XBUS_MEASUREMENT_PIPE, measurementSize);
		}
		else
			break; // No more data available to read.
	}
}

static void configureMtCommunicationInterface(void)
{
	mt.frequency(1000000);
	mt.format(8, 3);
	drdy.rise(&mtInterruptHandler);
}

/*!
 * \brief Send a message to the MT
 *
 * This function formats the message data and writes this to the MT SPI
 * interface. It does not wait for any response.
 */
static void sendMessage(XbusMessage const* m)
{
	uint8_t buf[64];
	size_t rawLength = XbusMessage_format(buf, m, XLLF_Spi);
	cs = 0;
	for (int i = 0; i < rawLength; ++i)
	{
		mt.write(buf[i]);
	}
	cs = 1;
}
#elif defined(MTI_USES_UART_INTERFACE)
/*!
 * \brief RX Interrupt handler for the MT serial port.
 *
 * Passes received data to an XbusParser to extract messages.
 */
static void mtLowLevelHandler(void)
{
	while (mt.readable())
	{
		XbusParser_parseByte(xbusParser, mt.getc());
	}
}

/*!
 * \brief Configure the serial port used for communication with the
 * motion tracker.
 */
static void configureMtCommunicationInterface(void)
{
	mt.baud(115200);
	mt.format(8, Serial::None, 1);
	mt.attach(mtLowLevelHandler, Serial::RxIrq);
}

/*!
 * \brief Send a message to the MT
 *
 * This function formats the message data and writes this to the MT serial
 * port. It does not wait for any response.
 */
static void sendMessage(XbusMessage const* m)
{
	uint8_t buf[64];
	size_t rawLength = XbusMessage_format(buf, m, XLLF_Uart);
	for (size_t i = 0; i < rawLength; ++i)
	{
		mt.putc(buf[i]);
	}
}
#endif


/*!
 * \brief Send a message to the MT and wait for a response.
 * \returns Response message from the MT, or NULL is no response received
 * within 500ms.
 *
 * Blocking behaviour is implemented by waiting for a response to be written
 * to the response queue by the XbusParser.
 */
static XbusMessage const* doTransaction(XbusMessage const* m)
{
	sendMessage(m);

	osEvent ev = g_responseQueue.get(500);
	return ev.status == osEventMessage ? (XbusMessage*)ev.value.p : NULL;
}

/*!
 * \brief RAII object to manage message memory deallocation.
 *
 * Will automatically free the memory used by an XbusMessage when going out
 * of scope.
 */
class XbusMessageMemoryManager
{
	public:
		XbusMessageMemoryManager(XbusMessage const* message)
			: m_message(message)
		{
		}

		~XbusMessageMemoryManager()
		{
			if (m_message)
			{
				if (m_message->data)
					deallocateMessageData(m_message->data);
				g_messagePool.free(const_cast<XbusMessage*>(m_message));
			}
		}

	private:
		XbusMessage const* m_message;
};

/*!
 * \brief Dump information from a message to the PC serial port.
 */
static void dumpResponse(XbusMessage const* response)
{
	switch (response->mid)
	{
		case XMID_GotoConfigAck:
			pc.printf("Device went to config mode.\r\n");
			break;

		case XMID_Error:
			pc.printf("Device error!");
			break;

		default:
			pc.printf("Received response MID=%X, length=%d\r\n", response->mid, response->length);
			break;
	}
}

/*!
 * \brief Send a command to the MT and wait for a response.
 * \param cmdId The XsMessageId of the command to send.
 *
 * Commands are simple messages without and payload data.
 */
static void sendCommand(XsMessageId cmdId)
{
	XbusMessage m = {cmdId};
	XbusMessage const* response = doTransaction(&m);
	XbusMessageMemoryManager janitor(response);

	if (response)
	{
		dumpResponse(response);
	}
	else
	{
		pc.printf("Timeout waiting for response.\r\n");
	}
}

/*!
 * \brief Handle a command from the PC
 *
 * The example application supports single character commands from the host
 * PC to switch between configuration and measurement modes.
 */
static void handlePcCommand(char cmd)
{
	switch (cmd)
	{
		case 'c':
			sendCommand(XMID_GotoConfig);
			break;

		case 'm':
			sendCommand(XMID_GotoMeasurement);
			break;
	}
}

/*!
 * \brief XbusParser callback function to handle received messages.
 * \param message Pointer to the last received message.
 *
 * In this example received messages are copied into one of two message
 * queues for later handling by the main thread. Data messages are put
 * in one queue, while all other responses are placed in the second queue.
 * This is done so that data and other messages can be handled separately
 * by the application code.
 */
static void mtMessageHandler(struct XbusMessage const* message)
{
	XbusMessage* m = g_messagePool.alloc();
	if (m)
	{
		memcpy(m, message, sizeof(XbusMessage));
		if (message->mid == XMID_MtData2)
		{
			g_dataQueue.put(m);
		}
		else
		{
			g_responseQueue.put(m);
		}
	}
	else if (message->data)
	{
		deallocateMessageData(message->data);
	}
}

/*!
 * \brief Configure the serial port used to communicate with the host PC.
 */
static void configurePcInterface(void)
{
	pc.baud(PC_UART_BAUDRATE);
	pc.format(8, Serial::None, 1);
}

/*!
 * \brief Read the device ID of the motion tracker.
 */
static uint32_t readDeviceId(void)
{
	XbusMessage reqDid = {XMID_ReqDid};
	XbusMessage const* didRsp = doTransaction(&reqDid);
	XbusMessageMemoryManager janitor(didRsp);
	uint32_t deviceId = 0;
	if (didRsp)
	{
		if (didRsp->mid == XMID_DeviceId)
		{
			deviceId = *(uint32_t*)didRsp->data;
		}
	}
	return deviceId;
}

/*!
 * \brief Sets MT output configuration.
 * \param conf Pointer to an array of OutputConfiguration elements.
 * \param elements The number of elements in the configuration array.
 *
 * The response from the device indicates the actual values that will
 * be used by the motion tracker. These may differ from the requested
 * parameters as the motion tracker validates the requested parameters
 * before applying them.
 */
static bool setOutputConfiguration(OutputConfiguration const* conf, uint8_t elements)
{
	XbusMessage outputConfMsg = {XMID_SetOutputConfig, elements, (void*)conf};
	XbusMessage const* outputConfRsp = doTransaction(&outputConfMsg);
	XbusMessageMemoryManager janitor(outputConfRsp);
	if (outputConfRsp)
	{
		if (outputConfRsp->mid == XMID_OutputConfig)
		{
			pc.printf("Output configuration set to:\r\n");
			OutputConfiguration* conf = (OutputConfiguration*)outputConfRsp->data;
			for (int i = 0; i < outputConfRsp->length; ++i)
			{
				pc.printf("\t%s: %d Hz\r\n", XbusMessage_dataDescription(conf->dtype), conf->freq);
				++conf;
			}
			return true;
		}
		else
		{
			dumpResponse(outputConfRsp);
		}
	}
	else
	{
		pc.printf("Failed to set output configuration.\r\n");
	}
	return false;
}

/*!
 * \brief Sets the motion tracker output configuration based on the function
 * of the attached device.
 *
 * The output configuration depends on the type of MTi-1 device connected.
 * An MTI-1 (IMU) device does not have an onboard orientation filter so
 * cannot output quaternion data, only inertial and magnetic measurement
 * data.
 * MTi-2 and MTi-3 devices have an onboard filter so can send quaternions.
 */
static bool configureMotionTracker(void)
{
	uint32_t deviceId = readDeviceId();

	if (deviceId)
	{
		pc.printf("Found device with ID: %08X.\r\n", deviceId);
		if (!XsDeviceId_isMtMk4_X(deviceId))
		{
			pc.printf("Device is not an MTi-1 series.\r\n");
			return false;
		}

		DeviceFunction function = XsDeviceId_getFunction(deviceId);
		pc.printf("Device is an MTi-%d: %s.\r\n", function, XsDeviceId_functionDescription(function));

		if (function == DF_IMU)
		{
			OutputConfiguration conf[] = {
				{XDI_PacketCounter, 65535},
				{XDI_SampleTimeFine, 65535},
				{XDI_Acceleration, 100},
				{XDI_RateOfTurn, 100},
				{XDI_MagneticField, 100}
			};
			return setOutputConfiguration(conf,
					sizeof(conf) / sizeof(OutputConfiguration));
		}
		else
		{
			OutputConfiguration conf[] = {
				{XDI_PacketCounter, 65535},
				{XDI_SampleTimeFine, 65535},
				{XDI_Quaternion, 100},
				{XDI_StatusWord, 65535}
			};
			return setOutputConfiguration(conf,
					sizeof(conf) / sizeof(OutputConfiguration));
		}
	}

	return false;
}

/*!
 * \brief Wait for a wakeup message from the MTi.
 * \param timeout Time to wait to receive the wakeup message.
 * \return true if wakeup received within timeout, else false.
 *
 * The MTi sends an XMID_Wakeup message once it has completed its bootup
 * procedure. If this is acknowledged by an XMID_WakeupAck message then the MTi
 * will stay in configuration mode. Otherwise it will automatically enter
 * measurement mode with the stored output configuration.
 */
bool waitForWakeup(uint32_t timeout)
{
	osEvent ev = g_responseQueue.get(timeout);
	if (ev.status == osEventMessage)
	{
		XbusMessage const* m = (XbusMessage const*)ev.value.p;
		XbusMessageMemoryManager janitor(m);
		return m->mid == XMID_Wakeup;
	}
	return false;
}

/*!
 * \brief Send wakeup acknowledge message to MTi.
 *
 * Sending a wakeup acknowledge will cause the device to stay in configuration
 * mode instead of automatically transitioning to measurement mode with the
 * stored output configuration.
 */
void sendWakeupAck(void)
{
	XbusMessage ack = {XMID_WakeupAck};
	sendMessage(&ack);
	pc.printf("Device ready for operation.\r\n");
}

#ifdef MTI_USES_UART_INTERFACE
/*!
 * \brief Restore communication with the MTi.
 *
 * On bootup the MTi will listen for a magic byte to signal that it should
 * return to default baudrate and output configuration. This can be used to
 * recover from a bad or unknown configuration.
 */
void restoreCommunication(void)
{
	pc.printf("Restoring communication with device... ");
	mtReset = 0;
	Thread::wait(1);
	mtReset = 1;

	do
	{
		mt.putc(0xDE);
	}
	while (!waitForWakeup(1));
	pc.printf("done\r\n");

	sendWakeupAck();
}
#endif

/*!
 * \brief Releases the MTi reset line and waits for a wakeup message.
 *
 * If no wakeup message is received within 1 second the restore communications
 * procedure is done to reset the MTi to default baudrate and output configuration.
 */
static bool wakeupMotionTracker(void)
{
	mtReset.write(1); // Release MT from reset.
	if (waitForWakeup(1000))
	{
		sendWakeupAck();
	}
	else
	{
#ifdef MTI_USES_UART_INTERFACE
		restoreCommunication();
#else
		pc.printf("Failed to communicate with MTi device\r\n");
		return true;
#endif
	}
	return true;
}

static void printIntroMessage(void)
{
	pc.printf("\r\n\r\n\r\n\r\n\r\n");
	pc.printf("MTi-1 series embedded example firmware.\r\n");
}

static void printUsageInstructions(void)
{
	pc.printf("\r\n");
	pc.printf("Press 'm' to start measuring and 'c' to return to config mode.\r\n");
}

/*!
 * \brief Output the contents of a data message to the PC serial port.
 */
static void printMessageData(struct XbusMessage const* message)
{
	if (!message)
		return;

	pc.printf("MTData2:");
	uint16_t counter;
	if (XbusMessage_getDataItem(&counter, XDI_PacketCounter, message))
	{
		pc.printf(" Packet counter: %5d", counter);
	}
	float ori[4];
	if (XbusMessage_getDataItem(ori, XDI_Quaternion, message))
	{
		pc.printf(" Orientation: (% .3f, % .3f, % .3f, % .3f)", ori[0], ori[1],
				ori[2], ori[3]);
	}
	float acc[3];
	if (XbusMessage_getDataItem(acc, XDI_Acceleration, message))
	{
		pc.printf(" Acceleration: (% .3f, % .3f, % .3f)", acc[0], acc[1], acc[2]);
	}
	float gyr[3];
	if (XbusMessage_getDataItem(gyr, XDI_RateOfTurn, message))
	{
		pc.printf(" Rate Of Turn: (% .3f, % .3f, % .3f)", gyr[0], gyr[1], gyr[2]);
	}
	float mag[3];
	if (XbusMessage_getDataItem(mag, XDI_MagneticField, message))
	{
		pc.printf(" Magnetic Field: (% .3f, % .3f, % .3f)", mag[0], mag[1], mag[2]);
	}
	uint32_t status;
	if (XbusMessage_getDataItem(&status, XDI_StatusWord, message))
	{
		pc.printf(" Status:%X", status);
	}
	pc.printf("\r\n");
}


int main(void)
{
	XbusParserCallback xbusCallback = {};
	xbusCallback.allocateBuffer = allocateMessageData;
	xbusCallback.deallocateBuffer = deallocateMessageData;
	xbusCallback.handleMessage = mtMessageHandler;

	xbusParser = XbusParser_create(&xbusCallback);
	configurePcInterface();
	configureMtCommunicationInterface();

	printIntroMessage();
	if (wakeupMotionTracker())
	{
		if (configureMotionTracker())
		{
			printUsageInstructions();
			for (;;)
			{
				while (pc.readable())
				{
					handlePcCommand(pc.getc());
				}

				osEvent ev = g_dataQueue.get(10);
				if (ev.status == osEventMessage)
				{
					XbusMessage const* data = (XbusMessage const*)ev.value.p;
					XbusMessageMemoryManager janitor(data);
					printMessageData(data);
				}
			}
		}
		else
		{
			pc.printf("Failed to configure motion tracker.\r\n");
			return -1;
		}
	}
}

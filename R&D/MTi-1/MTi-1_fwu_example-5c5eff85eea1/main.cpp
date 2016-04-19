/*! \file
	\copyright Copyright (C) Xsens Technologies B.V., 2015.

	Licensed under the Apache License, Version 2.0 (the "License"); you may not
	use this file except in compliance with the License. You may obtain a copy
	of the License at

	http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
	WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
	License for the specific language governing permissions and limitations
	under the License.

	\page Embedded firmware example
	The purpose of this example is to demonstrate how to update the firmware of a MTi-1 series module using the FwUpdate library.
	The example embeds an Xsens firmware file (xff) which is converted to a data array with srecord (http://srecord.sourceforge.net).
	The example is designed to run on a Nucleo F401 board. Porting to other mbed platforms should be easy but not all boards may
	have enough flash memory to hold the full xff

	\section Software setup
	Select the required bus mode by uncommenting one of the following defines at the top of main.cpp
	\verbatim
	BUS_MODE_SPI
	BUS_MODE_I2C
	BUS_MODE_UART
	\endverbatim
	Next, build the example and program it in the Nucleo F401 board.

	\section Development board setup
	Configure the required bus mode (UART, I2C, SPI) with the dip switches on the MTi-1 development board.
	(At the bottom of the development board a table with switch settings is shown)

	\section Hardware setup

	Unpower the board and make the following connections between the Nucleo F401 board and the
	MTi 1 series development board (depending on the selected bus mode, the unused connections may be
	left unconnected)

	Power supply and reset:
	\verbatim
	|----------------|---------------|-------------------------|
	|   Signal       | Nucleo F401   | Mti-1s Dev board header |
	|----------------|---------------|-------------------------|
	|    5V          |     5V        |       1                 |
	|    3V3         |     3V3       |       2                 |
	|    GND         |     GND       |       3                 |
	|   RESET        |     PC9       |       5                 |
	|----------------|---------------|-------------------------|
	\endverbatim

	Uart:
	\verbatim
	|----------------|---------------|-------------------------|
	|   Signal       | Nucleo F401   | Mti-1s Dev board header |
	|----------------|---------------|-------------------------|
	|  Tx of Nucleo  |    PA9        |       11                |
	|  Tx of MTi     |    PA10       |       9                 |
	|----------------|---------------|-------------------------|
	\endverbatim

	I2C:
	\verbatim
	|----------------|---------------|-------------------------|
	|   Signal       | Nucleo F401   | Mti-1s Dev board header |
	|----------------|---------------|-------------------------|
	|    SCL         |    PB8        |       9                 |
	|    SDA         |    PB9        |       11                |
	|    DRDY        |    PB3        |       15                |
	|    ADD0        |    PB13       |       17                |
	|    ADD1        |    PB14       |       19                |
	|    ADD2        |    PB15       |       21                |
	|----------------|---------------|-------------------------|
	\endverbatim

	SPI:
	\verbatim
	|----------------|---------------|-------------------------|
	|   Signal       | Nucleo F401   | Mti-1s Dev board header |
	|----------------|---------------|-------------------------|
	|    SCK         |    PB13       |       17                |
	|    MISO        |    PB14       |       19                |
	|    MOSI        |    PB15       |       21                |
	|    nCS         |    PB6        |       23                |
	|    DRDY        |    PB3        |       15                |
	|----------------|---------------|-------------------------|
	\endverbatim

	\section Connection with host PC
	Connect the Nucleo board with a USB cable to the host PC. Find out on which
	COM port the Nucleo board is mapped by the OS, and open a terminal emulator (e.g. PuTTY)
	on this port. The default baud rate of the example is 921600. Pressing 'h' in the terminal
	window should return the following usage text:

	\verbatim
	Embedded firmware updater example
	Interface: SPI

	h: Print this text
	c: GotoConfig
	m: GotoMeasurement
	r: Soft reset the module
	b: GotoBootloader
	v: Request firmware revision
	d: Request deviceId
	u: Start firmware update (make sure module is in bootloader mode)
	x: Hard reset the module and make it stay in bootloader
	\endverbatim

	After power-up the module automatically goes to measurement mode. In this mode
	only a limited set of commands is supported. Therefore, the module must first be
	switched to config mode by pressing 'c'. The module should response with XMID_GotoConfigAck.

	Next, the firmware revision can be requested by pressing 'v'. This should return
	the firmware revision in the format Major.Minor.Patch. If the Major version number is 255, the
	module is in bootloader mode. Any other Major version number means that the MTi-1s main firmware is
	running. In this case the module must be set in bootloader mode	before a firmware update can be done.
	The module can be placed in bootloader mode by pressing either the 'b' or 'x' key
 */


#include "mbed.h"
#include "rtos.h"
#include "board.h"
#include "xbusmessage.h"
#include "xbusparser.h"
#include "xbusmessageid.h"
#include "mtinterface_mtssp.h"
#include "mtinterface_uart.h"
#include "mtssp_i2c_driver.h"
#include "mtssp_spi_driver.h"
#include "fwupdate.h"
#include "xffdata.h"


/*!	\brief Bus mode to use by this example.
*/
//#define BUS_MODE_SPI
#define BUS_MODE_I2C
//#define BUS_MODE_UART

static void updateUartBaudrate();

/*!	\brief Instance of MtInterface. Should be initialized by createMtInterface().
*/
static MtInterface* mtInterface = 0;


/*!	\brief Serial object for communicating with the PC.
*/
static Serial pc(PC_TX, PC_RX);


/*!	\brief Reset line towards the module. Pulling this line down keeps the module in reset.
*/
static DigitalInOut resetLine(MT_RESET, PIN_INPUT, OpenDrain, 1);


/*!	\brief Instance of the firmware updater
*/
static FwUpdate *g_fwUpdate = NULL;


/*!	\brief Create an instance of MtInterface.
*/
static void createMtInterface()
{
#ifdef BUS_MODE_SPI
	MtsspDriver* mtsspDriver = new MtsspSpiDriver;
	mtInterface = new MtInterfaceMtssp(mtsspDriver);
#endif

#ifdef BUS_MODE_I2C
	MtsspDriver* mtsspDriver = new MtsspI2cDriver;
	mtInterface = new MtInterfaceMtssp(mtsspDriver);
#endif

#ifdef BUS_MODE_UART
	mtInterface = new MtInterfaceUart(115200);
#endif
}


/*!	\brief Callback function for FwUpdate for reading data from the xff (xsens firmware file) file.
	\param buffer Target buffer in which the xff data should be written
	\param offset Offset in the xff file where reading should start
	\param length Number of bytes which is requested
	\returns Number of bytes which is actually written to the buffer
*/
static int readXffData(uint8_t *buffer, int offset, int length)
{
	int n;
	for (n = 0; n < length; n++)
	{
		if (offset + n == g_xffData_length)
			break;
		buffer[n] = g_xffData[offset + n];
	}
	return n;
}


/*!	\brief Callback function for FwUpdate for signaling that a firmware update has completed.
	\param result Result of the firmware update, either FWU_Success or FWU_Failed
*/
static void readyHandler(FWU_Result result)
{
	if (result == FWU_Success)
		pc.printf("Firmware update ready\r\n");
	else if (result == FWU_Failed)
		pc.printf("Firmware update failed\r\n");
	
	if (mtInterface->busFormat() == XBF_Uart)
		updateUartBaudrate();
}


/*! \brief Sends a XMID_GotoConfig message to the Module
*/
static void gotoConfig()
{
	XbusMessage xbusMessage(XMID_GotoConfig);
	mtInterface->sendXbusMessage(&xbusMessage);
}

/*!	\brief Handles xbus messages from the module.
	\param xbusMessage The xbus message received from the module

	Messages with message identifier XMID_FirmwareUpdate are passed to the firmware updater, other messages are just printed.
*/
static void handleXbusMessage(XbusMessage *xbusMessage)
{
	switch (xbusMessage->m_mid)
	{
	case XMID_Wakeup:
	{
		//If the wakeup message is received we force the module in config mode
		gotoConfig();
		pc.printf("XMID_Wakeup\r\n");
	} break;

	case XMID_GotoConfigAck:
	{
		pc.printf("XMID_GotoConfigAck\r\n");
	} break;

	case XMID_GotoMeasurementAck:
	{
		pc.printf("XMID_GotoMeasurementAck\r\n");
	} break;

	case XMID_ResetAck:
	{
		pc.printf("XMID_ResetAck\r\n");
	} break;

	case XMID_GotoBootLoaderAck:
	{
		pc.printf("XMID_GotoBootLoaderAck\r\n");
	} break;

	case XMID_Error:
	{
		pc.printf("XMID_Error\r\n");
	} break;

	case XMID_FirmwareUpdate:
	{
		if (g_fwUpdate != NULL)
		{
			FwUpdate_handleXbus(g_fwUpdate, xbusMessage);
		}
	} break;

	case XMID_FirmwareRevision:
	{
		pc.printf("XMID_FirmwareRevision: %d.%d.%d\r\n", xbusMessage->m_data[0], xbusMessage->m_data[1], xbusMessage->m_data[2]);
	} break;

	case XMID_DeviceId:
	{
		uint32_t deviceId = (xbusMessage->m_data[0] << 24) | (xbusMessage->m_data[1] << 16) | (xbusMessage->m_data[2] << 8) | xbusMessage->m_data[3];
		pc.printf("XMID_DeviceId: %08X\r\n", deviceId);
	} break;

	case XMID_MtData2:
	{
		pc.printf("XMID_MtData2\r\n");
	} break;

	default:
	{
		pc.printf("Unhandled xbus message, mid = 0x%02X, len = %d", xbusMessage->m_mid, xbusMessage->m_length);
		if (xbusMessage->m_length > 0)
		{
			pc.printf(", data = ");
			for (int n = 0; n < xbusMessage->m_length; n++)
			{
				pc.printf("%02X ", xbusMessage->m_data[n]);
			}
		}
		pc.printf("\r\n");
	}
	}
}


/*!	\brief Reset the module via the hardware reset line.
	\param stayInBootloader: If stayInBootloader is true, an XMID_GotoBootLoader is sent immediately after the reset.

	By sending an XMID_GotoBootLoader within 100 ms after reset the module stays in bootloader mode
	instead of booting the application. This can be used if the normal GotoBootloader command fails to put
	the module in bootloader mode, e.g. because of a faulty firmware.
*/
static void hardwareReset(bool stayInBootloader)
{
	resetLine = 1;
	resetLine.output();
	wait(0.001);
	resetLine = 0;
	wait(0.001);
	resetLine.input();	// (configure the line as input because the OpenDrain setting of the mbed gpio pin does not seem to work correctly)		

	if (stayInBootloader)
	{
		wait(0.02);
		XbusMessage xbusMessage(XMID_GotoBootLoader);
		mtInterface->sendXbusMessage(&xbusMessage);
	} else
	{
		wait(0.2);
	}
}


/*!	\brief Try to find the correct uart baudrate
*/
static void updateUartBaudrate()
{
	if (mtInterface->busFormat() != XBF_Uart)
		return;
	
	pc.printf("Detecting uart baudrate of module\r\n");
	
	const int nofBaudrates = 2;
	int baudrates[nofBaudrates] = {921600, 115200};
	
	bool success = false;
	for (int n = 0; n < nofBaudrates; n++)
	{
		((MtInterfaceUart*)mtInterface)->setBaudrate(baudrates[n]);
		pc.printf("Baudrate set to %d\r\n", baudrates[n]);
		XbusMessage gotoConfigMessage(XMID_GotoConfig);
		XbusMessage* result = mtInterface->sendAndWait(&gotoConfigMessage);
		success = result && result->m_mid == XMID_GotoConfigAck;
		mtInterface->releaseXbusMessage(result);
		if (success)
			break;
	}
	if (success)
		pc.printf("Detecting uart baudrate of module done\r\n");
	else
		pc.printf("Detecting uart baudrate of module failed\r\n");
}

bool isInBootloader()
{
	XbusMessage fwRevisionMessage(XMID_ReqFirmwareRevision);
	XbusMessage* ack = mtInterface->sendAndWait(&fwRevisionMessage);

	bool result = false;
	if (ack && ack->m_mid == XMID_FirmwareRevision)
		result = ack->m_data[0] == 255;

	mtInterface->releaseXbusMessage(ack);
	return result;
}

/*!	\brief C-wrapper for sendXbusMessage callback function of FwUpdate.
*/
static void sendXbusMessageWrapper(XbusMessage const* xbusMessage)
{
	if (mtInterface)
		mtInterface->sendXbusMessage(xbusMessage);
}


/*!	\brief Helper function for converting a XbusLowLevelFormat to string.
*/
static char* lowLevelFormatToString(XbusBusFormat format)
{
	switch (format)
	{
	case XBF_I2c:
		return "I2C";
	case XBF_Spi:
		return "SPI";
	case XBF_Uart:
		return "UART";
	}
	return "unknown";
}


/*!	\brief Print usage info of this example.
*/
static void printUsageInfo()
{
	pc.printf("\r\nEmbedded firmware updater example\r\n");
	pc.printf("Interface: %s\r\n\r\n", lowLevelFormatToString(mtInterface->busFormat()));
	pc.printf("h: Print this text\r\n");
	pc.printf("c: GotoConfig\r\n");
	pc.printf("m: GotoMeasurement\r\n");
	pc.printf("r: Soft reset the module\r\n");
	pc.printf("b: GotoBootloader\r\n");
	pc.printf("v: Request firmware revision\r\n");
	pc.printf("d: Request deviceId\r\n");
	pc.printf("u: Start firmware update (make sure module is in bootloader mode)\r\n");
	pc.printf("x: Hard reset the module and make it stay in bootloader\r\n");
	pc.printf("\r\n");
}

/*!	\brief Main entry point of the embedded firmware updater example.
*/
int main()
{
	resetLine.mode(OpenDrain);
	// Configure communication with the pc:
	pc.baud(921600);
	pc.format(8, Serial::None, 1);
	pc.printf("\r\n\r\n\r\n--------------------------------------\r\n");

	// Initialize the mtInterface object for communicating with the module:
	createMtInterface();
	
	// If Uart mode is used update the baudrate to match the baudrate of the module:
	if (mtInterface->busFormat() == XBF_Uart)
		updateUartBaudrate();

	// Reset the module
	hardwareReset(false);

	// Initialize the firmware updater:
	g_fwUpdate = new FwUpdate();
	uint8_t* fwuTxBuffer = new uint8_t[FWU_REQUIRED_TXBUFFER_SIZE];
	g_fwUpdate->m_readXffData = readXffData;
	g_fwUpdate->m_sendXbusMessage = sendXbusMessageWrapper;
	g_fwUpdate->m_readyHandler = readyHandler;
	g_fwUpdate->m_txBuffer = fwuTxBuffer;
	FwUpdate_init(g_fwUpdate);

	printUsageInfo();

	// Main loop:
	bool running = true;
	while (running)
	{
		mtInterface->process();

		XbusMessage* xbusMessage = mtInterface->getXbusMessage();
		bool updateUartBaudrateRequired = false;
		if (xbusMessage != NULL)
		{
			handleXbusMessage(xbusMessage);
			updateUartBaudrateRequired = (mtInterface->busFormat() == XBF_Uart && (xbusMessage->m_mid == XMID_ResetAck || xbusMessage->m_mid == XMID_GotoBootLoaderAck));
			mtInterface->releaseXbusMessage(xbusMessage);
		}
		
		if (updateUartBaudrateRequired)
		{
			wait(0.2);
			updateUartBaudrate();
		}

		if (pc.readable())
		{
			char cmd = pc.getc();
			switch (cmd)
			{
			case 'h':
			{
				printUsageInfo();
			} break;

			case 'c':
			{
				gotoConfig();
			} break;

			case 'm':
			{
				XbusMessage xbusMessage(XMID_GotoMeasurement);
				mtInterface->sendXbusMessage(&xbusMessage);
			} break;

			case 'r':
			{
				XbusMessage xbusMessage(XMID_Reset);
				mtInterface->sendXbusMessage(&xbusMessage);
			} break;

			case 'b':
			{
				XbusMessage xbusMessage(XMID_GotoBootLoader);
				mtInterface->sendXbusMessage(&xbusMessage);
			} break;

			case 'v':
			{
				XbusMessage xbusMessage(XMID_ReqFirmwareRevision);
				mtInterface->sendXbusMessage(&xbusMessage);
			} break;

			case 'd':
			{
				XbusMessage xbusMessage(XMID_ReqDid);
				mtInterface->sendXbusMessage(&xbusMessage);
			} break;

			case 'u':
			{
				if (isInBootloader())
				{
					pc.printf("Starting firmware update\r\n");
					FwUpdate_init(g_fwUpdate);
					FwUpdate_start(g_fwUpdate);
				}
				else
				{
					pc.printf("Firmware update not possible now. First switch module to bootloader.\r\n");
				}
			} break;

			case 'x':
			{
				hardwareReset(true);
			} break;
			}
		}
	}

	delete g_fwUpdate;
	delete[] fwuTxBuffer;
	return -1;
}


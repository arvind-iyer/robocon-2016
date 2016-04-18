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
#ifndef FWUPDATE_H
#define FWUPDATE_H

#include "xbusmessage.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FWU_REQUIRED_TXBUFFER_SIZE 300

/*!	\brief Result value
*/
typedef enum
{
	FWU_Success,
	FWU_Failed
} FWU_Result;


/*!	\brief Definition of a xff section header
*/
typedef struct
{
	uint32_t m_globalId;
	uint32_t m_sectionSize;
	uint8_t m_firmwareRevision[3];
	uint8_t m_xffVersion[2];
	uint8_t m_chipId;
	uint8_t m_numberOfSections;
	uint8_t m_addressLength;
	uint16_t m_pageSize;
	uint16_t m_sliceSize;
} XffHeader;


/*!	\brief Internal state
*/
typedef enum
{
	STATE_Idle,
	STATE_Start,
	STATE_WaitReady,
	STATE_WaitHeaderResult,
	STATE_WaitSliceReady,
	STATE_WaitPageOk,
	STATE_WaitPageReady
} FWU_State;



/*!	\brief FwUpdate object definition
*/
typedef struct
{
	/*	External dependencies. Host should fill in these members */

	/*!	\brief Callback function by which FwUpdate requests for xff data
		\param buffer Target buffer in which the xff data should be written by the host
		\param offset Offset in the xff file where reading should start
		\param length Number of bytes which is requested
		\returns Number of bytes which is actually written to the buffer
	*/
	int (*m_readXffData)(uint8_t *buffer, int offset, int length);

	/*!	\brief Callback function via which FwUpdate can send xbus messages to the module
		\param xbusMessage Xbus message that should be send to the module
	*/
	void (*m_sendXbusMessage)(struct XbusMessage const* xbusMessage);

	/*!	\brief Callback function by which FwUpdate notifies the host that a firmware update has finished
		\param result FWU_Success or FWU_Failed
	*/
	void (*m_readyHandler)(FWU_Result result);

	/*!	\brief Memory needed by the FwUpdate. Host must allocate a block of memory
		of size FWU_REQUIRED_TXBUFFER_SIZE
	*/
	uint8_t *m_txBuffer;

	/*	State variables for internal use (the user must not touch these) */
	FWU_State m_state;
	XffHeader m_xffHeader;
	int m_nofPages;
	int m_nofSlicesPerPage;
	int m_pageCounter;
	int m_sliceCounter;
	int m_readIndex;
	uint8_t m_endOfFile;
} FwUpdate;


void FwUpdate_init(FwUpdate *thisPtr);

void FwUpdate_start(FwUpdate *thisPtr);

void FwUpdate_handleXbus(FwUpdate *thisPtr, struct XbusMessage *xbusMessage);


#ifdef __cplusplus
}
#endif /* extern "C" */


#endif /* FWUPDATE_H */

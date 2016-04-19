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
#ifndef XBUSMESSAGEID_H
#define XBUSMESSAGEID_H


/*! \brief Xbus message IDs. */
enum XsMessageId
{
	XMID_Wakeup              = 0x3E,
	XMID_WakeupAck           = 0x3F,
	XMID_ReqDid              = 0x00,
	XMID_DeviceId            = 0x01,
	XMID_GotoConfig          = 0x30,
	XMID_GotoConfigAck       = 0x31,
	XMID_GotoMeasurement     = 0x10,
	XMID_GotoMeasurementAck  = 0x11,
	XMID_MtData2             = 0x36,
	XMID_ReqOutputConfig     = 0xC0,
	XMID_SetOutputConfig     = 0xC0,
	XMID_OutputConfig        = 0xC1,
	XMID_Reset               = 0x40,
	XMID_ResetAck            = 0x41,
	XMID_Error               = 0x42,
	XMID_ToggleIoPins        = 0xBE,
	XMID_ToggleIoPinsAck     = 0xBF,
	XMID_FirmwareUpdate      = 0xF2,
	XMID_GotoBootLoader      = 0xF0,
	XMID_GotoBootLoaderAck   = 0xF1,
	XMID_ReqFirmwareRevision = 0x12,
	XMID_FirmwareRevision    = 0x13


};



#endif // XBUSMESSAGEID_H

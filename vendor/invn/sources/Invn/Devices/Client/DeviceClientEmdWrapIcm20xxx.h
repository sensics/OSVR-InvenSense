/*
 * ________________________________________________________________________________________________________
 * Copyright (c) 2015-2015 InvenSense Inc. All rights reserved.
 *
 * This software, related documentation and any modifications thereto (collectively “Software”) is subject
 * to InvenSense and its licensors' intellectual property rights under U.S. and international copyright
 * and other intellectual property rights laws.
 *
 * InvenSense and its licensors retain all intellectual property and proprietary rights in and to the Software
 * and any use, reproduction, disclosure or distribution of the Software without an express license agreement
 * from InvenSense is strictly prohibited.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE SOFTWARE IS
 * PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * INVENSENSE BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 * ________________________________________________________________________________________________________
 */

#ifndef _IDD_CLIENT_DEVICE_EMD_WRAP_ICM20XXX_H_
#define _IDD_CLIENT_DEVICE_EMD_WRAP_ICM20XXX_H_

#include "Invn/InvExport.h"

#include "Invn/Devices/DeviceEmdWrapIcm20xxx.h"
#include "Invn/Devices/Client/DeviceClient.h"
#include "Invn/Devices/HostAdapter/SmartUartAdapter.h"

#include <string>

//-----------------------------------------------------------------------------

class INV_EXPORT DeviceClientEmdWrapIcm20xxx : public DeviceClient
{
public:

	DeviceClientEmdWrapIcm20xxx(const std::string & com_port = "");
	virtual ~DeviceClientEmdWrapIcm20xxx();

	void setup();
	void cleanup();
	virtual bool setSensorConfigImpl(int sensor, const std::string & settings, const std::string & value);
	virtual bool getSensorConfigImpl(int sensor, const std::string & settings, std::string & value);

	int doSerialWrite(const uint8_t * data, unsigned len);
	int doSerialRead(uint8_t * data, unsigned len);

	static int serialWriteCb(const uint8_t * data, unsigned len, void * cookie);
	static int serialReadCb(uint8_t * data, unsigned len, void * cookie);

protected:

	inv_device_emd_wrap_icm20xxx_t _device_emd_wrap_icm20xxx;

private:

	enum EmdWrapIcm20xxxState {
		EMD_WRAP_ICM20XXX_STATE_CREATED,
		EMD_WRAP_ICM20XXX_STATE_SETUP_DONE,
		EMD_WRAP_ICM20XXX_STATE_CLEANUP_DONE,
		EMD_WRAP_ICM20XXX_STATE_DESTROYED,
	};

	// disable annoying warnings related to usage of STL templates in exported class
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251)
#endif
	std::string _com_port;
#ifdef _MSC_VER
#pragma warning( pop )
#endif

	UartHostAdapter *    _serial_impl;
	uint8_t *            _read_buffer;
	uint8_t *            _write_buffer;
	EmdWrapIcm20xxxState _state;
};

//-----------------------------------------------------------------------------

#endif /* _IDD_CLIENT_DEVICE_EMD_WRAP_ICM20XXX_H_ */

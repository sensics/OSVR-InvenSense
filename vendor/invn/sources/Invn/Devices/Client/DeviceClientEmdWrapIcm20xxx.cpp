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

#include "DeviceClientEmdWrapIcm20xxx.h"

#include "Invn/EmbUtils/Message.h"
#include "Invn/HostUtils/Os.h"

#include "Invn/Devices/HostAdapter/UartHostAdapter.h"
#include "Invn/Devices/HostAdapter/SmartUartAdapter.h"

#include <string>

//-----------------------------------------------------------------------------

#define BUFFER_SIZE        (128*1024)

int DeviceClientEmdWrapIcm20xxx::serialWriteCb(const uint8_t * data, unsigned len, void * cookie)
{
	DeviceClientEmdWrapIcm20xxx * self = (DeviceClientEmdWrapIcm20xxx *)cookie;
	return self->doSerialWrite(data, len);
}

int DeviceClientEmdWrapIcm20xxx::serialReadCb(uint8_t * data, unsigned len, void * cookie)
{
	DeviceClientEmdWrapIcm20xxx * self = (DeviceClientEmdWrapIcm20xxx *)cookie;
	return self->doSerialRead(data, len);
}

int DeviceClientEmdWrapIcm20xxx::doSerialWrite(const uint8_t * data, unsigned len)
{
	return _serial_impl->write(data, len);
}

int DeviceClientEmdWrapIcm20xxx::doSerialRead(uint8_t * data, unsigned len)
{
	return _serial_impl->read(data, len);
}

DeviceClientEmdWrapIcm20xxx::DeviceClientEmdWrapIcm20xxx(const std::string & com_port)
	: DeviceClient(0, &_device_emd_wrap_icm20xxx.base)
	, _com_port(com_port)
	, _serial_impl(new SmartUartAdapter(_com_port))
	, _read_buffer(new uint8_t[BUFFER_SIZE])
	, _write_buffer(new uint8_t[BUFFER_SIZE])
{
	struct inv_device_emd_wrap_icm20xxx_serial emd_serial = {
		DeviceClientEmdWrapIcm20xxx::serialReadCb,_read_buffer, BUFFER_SIZE,
		DeviceClientEmdWrapIcm20xxx::serialWriteCb, _write_buffer, BUFFER_SIZE
	};

	_state = EMD_WRAP_ICM20XXX_STATE_CREATED;
	inv_device_emd_wrap_icm20xxx_init(&_device_emd_wrap_icm20xxx, &_listener, &emd_serial, this);
}

DeviceClientEmdWrapIcm20xxx::~DeviceClientEmdWrapIcm20xxx()
{
	cleanup();
	delete _serial_impl;
	delete [] _read_buffer;
	delete [] _write_buffer;
	_state = EMD_WRAP_ICM20XXX_STATE_DESTROYED;
}

void DeviceClientEmdWrapIcm20xxx::setup()
{
	int rc = 0;

	/* Open the requested COM port with no handshake */
	_serial_impl->setFlowcontrol(UartHostAdapter::INVN_UART_HOST_ADAPTER_FLOW_NONE);
	_serial_impl->open();

	/* If HW handshake is supported, close and re-open the COM port with HW handshake enabled */
	if(inv_device_emd_wrap_icm20xxx_is_hw_handshake_supported(&_device_emd_wrap_icm20xxx) == 1) {
		_serial_impl->close();
		_serial_impl->setFlowcontrol(UartHostAdapter::INVN_UART_HOST_ADAPTER_FLOW_RTS_CTS);
		_serial_impl->open();
	}

	if((rc = inv_device_emd_wrap_icm20xxx_setup(&_device_emd_wrap_icm20xxx)) != 0) {
		_serial_impl->close();
		handleError(rc, "EmdWrapper firmware did not start properly");
	}

	_state = EMD_WRAP_ICM20XXX_STATE_SETUP_DONE;
}

void DeviceClientEmdWrapIcm20xxx::cleanup()
{
	if (_state == EMD_WRAP_ICM20XXX_STATE_SETUP_DONE) {
		inv_device_emd_wrap_icm20xxx_cleanup(&_device_emd_wrap_icm20xxx);
		_state = EMD_WRAP_ICM20XXX_STATE_CLEANUP_DONE;
	}
	_serial_impl->close();
}

bool DeviceClientEmdWrapIcm20xxx::setSensorConfigImpl(int sensor, const std::string & settings,
		const std::string & value)
{
	unsigned i;

	INV_MSG(INV_MSG_LEVEL_DEBUG, "Updating EmdWrapper sensor %s configuration(%s=%s)",
			inv_sensor_str(sensor), settings.c_str(), value.c_str());

	if( (settings == "offset") || (settings == "offsets") || (settings == "bias") ||
		(settings == "fnm_bias")) {
		float bias [3];
		int32_t q16_bias[3];
		// parse input string with a oldie scanf
		int rc = sscanf(value.c_str(), "%f %f %f", &bias[0], &bias[1], &bias[2]);
		if(rc == 3) {
			/* float to Q16 conversion */
			for (i = 0; i < 3; i++)
			{
				q16_bias[i] = ( (int32_t)  ((float)(bias[i])*(1ULL << 16) + ( (bias[i]>=0)-0.5f )) );
			}
			if (settings == "fnm_bias")
				rc = inv_device_set_sensor_config(_device, sensor, INV_SENSOR_CONFIG_FNM_OFFSET, q16_bias, sizeof(q16_bias));
			else
				rc = inv_device_set_sensor_config(_device, sensor, INV_SENSOR_CONFIG_OFFSET, q16_bias, sizeof(q16_bias));
			handleError(rc, "Fail to set bias");
			if (rc != 0)
				return false;
			return true;
		} else {
			handleError(INV_ERROR_BAD_ARG, "Invalid argument for 'bias' setting");
			return false;
		}
	}
	else if( (settings == "predgrv") || (settings == "pred_grv") || (settings == "pred_quat") ) {
		int8_t raw[5];
		/* Use of a temporary int to stop sscanf() warning about "%d" storing to an int8_t var */
		int tmp1;
		inv_sensor_config_pred_grv config;
		/* expected input is "<predictive_algorithm> <predictive_latency>" */
		int rc = sscanf(value.c_str(), "%d %d", &tmp1, &config.latency);
		if (rc == 2) {
			config.method = (int8_t)tmp1;
			raw[0] = config.method;
			for (int i=0; i<sizeof(config.latency);i++)
				raw[i+1] = ((int8_t*)&config.latency)[i];
			rc = inv_device_set_sensor_config(_device, sensor, INV_SENSOR_CONFIG_PRED_GRV, raw, sizeof(raw));
			handleError(rc, "Fail to set predictive grv parameters");
			if (rc != 0)
				return false;
			return true;
		} else {
			handleError(INV_ERROR_BAD_ARG, "Invalid argument for 'predgrv' setting");
			return false;
		}
	}
	else if( (settings == "bt_state") ) {
		int state;
		inv_sensor_config_bt_state_t bt_state;
		int rc = sscanf(value.c_str(), "%d", &state);
		if (rc == 1) {
			bt_state.enable = (state != 0); /*make it 0 or 1*/
			rc = inv_device_set_sensor_config(_device, sensor, INV_SENSOR_CONFIG_BT_STATE, &bt_state, sizeof(bt_state));
			handleError(rc, "Fail to set bias tracker parameters");
			if (rc != 0)
				return false;
			return true;
		} else {
			handleError(INV_ERROR_BAD_ARG, "Invalid argument for 'state' setting");
			return false;
		}
	}
	else {
		return setSensorConfigCommon(sensor, settings, value);
	}
}

bool DeviceClientEmdWrapIcm20xxx::getSensorConfigImpl(int sensor, const std::string & settings,
		std::string & value)
{
	unsigned i;
	int size;
	char temp[128];

	INV_MSG(INV_MSG_LEVEL_DEBUG, "Retrieving EmdWrapper sensor %s configuration(%s)",
			inv_sensor_str(sensor), settings.c_str());

	if( (settings == "offset") || (settings == "offsets") || (settings == "bias") ||
		(settings == "fnm_bias") ) {
		int config_type;
		int addr = 0;
		float bias[3];
		intq16_t *q16_bias;
		VSensorConfigOffset cfg;

		if (settings == "fnm_bias")
			config_type = INV_SENSOR_CONFIG_FNM_OFFSET;
		else
			config_type = INV_SENSOR_CONFIG_OFFSET;

		if((size = inv_device_get_sensor_config(_device, sensor, config_type, &cfg.vect[0], 3 * sizeof(intq16_t))) >= 0) {
			temp[0] = '\0';
			value.clear();
			q16_bias = (intq16_t*) &cfg.vect[0];
			/* Q16 to float conversion */
			for (i = 0; i < 3; i++) {
				bias[i] = ( (float)  (int32_t)(q16_bias[i]) / (float)(1ULL << 16) );
				addr += sprintf(&temp[addr], "%f ", bias[i]);
			}
			value = temp;
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------

void inv_sleep_ms(unsigned ms)
{
	inv_os_sleep(ms);
}

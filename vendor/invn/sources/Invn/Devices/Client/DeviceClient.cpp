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

#include "DeviceClient.h"
#include "SensorEventsListener.h"
#include "HostAdapterClient.h"

#include "Invn/EmbUtils/ErrorHelper.h"
#include "Invn/EmbUtils/Message.h"

#include "Invn/HostUtils/Os.h"
#include "Invn/HostUtils/BinaryFile.h"
#include "Invn/HostUtils/DmpImageConverter.h"

#include <cassert>
#include <stdexcept>
#include <cstring>

// Poco
#include "Poco/Mutex.h"

#define DEVICE_INSTANCE_DESC	  "Device instance fail"
#define DEVICE_STARTING_DESC      "Starting device fail"
#define DEVICE_SETTING_CMD_DESC   "Device setting command fail"
#define DEVICE_POLLING_DESC       "Device polling answer fail"

//-----------------------------------------------------------------------------
// DeviceLocker
//-----------------------------------------------------------------------------

struct INV_EXPORT DeviceLocker::Impl { Poco::FastMutex mutex; };
DeviceLocker::DeviceLocker() : _impl(new Impl) { }
DeviceLocker::~DeviceLocker() { delete _impl; }
void DeviceLocker::lock() { _impl->mutex.lock(); }
void DeviceLocker::unlock() { _impl->mutex.unlock(); }

DeviceLocker::ScopedLock::ScopedLock(DeviceLocker* locker)
: _locker(locker)
{
	if (locker)
		locker->lock();
}

DeviceLocker::ScopedLock::~ScopedLock()
{
	if (_locker)
		_locker->unlock();
}

//-----------------------------------------------------------------------------
// DeviceClient
//-----------------------------------------------------------------------------

DeviceClient::DeviceClient(HostAdapterClient* serif, inv_device_t * dev)
: _serif(serif)
, _device(dev)
, _debugger_hook(0)
, _locker(0)
, _listener_ref(0)
{
	inv_sensor_listener_init(&_listener, sensorEventCallback, this);
}

DeviceClient::~DeviceClient()
{
}

void DeviceClient::registerDebuggerHook(DeviceDebuggerHook * dbg_hook)
{
	DeviceLocker::ScopedLock lock(_locker);
	_debugger_hook = dbg_hook;
}

void DeviceClient::callDebuggerHook()
{
	// don't lock (we should be in same context as setup() function, so it's already locked).
	if(_debugger_hook) {
		_debugger_hook->waitForDebugger(this);
	}
}

void DeviceClient::handleError(int rc, const char * desc)
{
	if (rc < 0) {
		if(desc[0] != '\0') {
			throw std::runtime_error(std::string(inv_error_str(rc)) + ": " + std::string(desc));
		}
		else {
			throw std::runtime_error(inv_error_str(rc));
		}
	}
}

int DeviceClient::whoami(void)
{
	uint8_t whoami;

	DeviceLocker::ScopedLock lock(_locker);
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Reading WHOAMI...");
	handleError(inv_device_whoami(_device, &whoami), "Device Identification fail - Chip ID not recognized");
	return whoami;
}

void DeviceClient::getFwInfo(struct inv_fw_version & version)
{
	DeviceLocker::ScopedLock lock(_locker);
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Getting fw info...");
	handleError(inv_device_get_fw_info(_device, &version), "Device Identification fail - FW info not available");
}

void DeviceClient::setRunningState(bool state)
{
	DeviceLocker::ScopedLock lock(_locker);
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Set running state...");
	handleError(inv_device_set_running_state(_device, state), DEVICE_STARTING_DESC);
}

bool DeviceClient::pingSensor(int sensor)
{
	DeviceLocker::ScopedLock lock(_locker);
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Ping sensor %s...", inv_sensor_2str(sensor));

	const int rc = inv_device_ping_sensor(_device, sensor);
	if(rc == INV_ERROR || rc == INV_ERROR_BAD_ARG)
		return false;
	handleError(rc, DEVICE_SETTING_CMD_DESC);

	return true;
}

void DeviceClient::startSensor(int sensor)
{
	DeviceLocker::ScopedLock lock(_locker);
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Starting sensor %s...", inv_sensor_2str(sensor));
	handleError(inv_device_start_sensor(_device, sensor), DEVICE_SETTING_CMD_DESC);
}

void DeviceClient::stopSensor(int sensor)
{
	DeviceLocker::ScopedLock lock(_locker);
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Stopping sensor %s...", inv_sensor_2str(sensor));
	handleError(inv_device_stop_sensor(_device, sensor), DEVICE_SETTING_CMD_DESC);
}

void DeviceClient::setSensorPeriodUs(int sensor, unsigned long period)
{
	DeviceLocker::ScopedLock lock(_locker);
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Setting period %luus for sensor %s...",
			period, inv_sensor_2str(sensor));
	handleError(inv_device_set_sensor_period_us(_device, sensor, period), DEVICE_SETTING_CMD_DESC);
}

void DeviceClient::setSensorPeriod(int sensor, unsigned long period)
{
	setSensorPeriodUs(sensor, period*1000);
}

void DeviceClient::setSensorTimeout(int sensor, unsigned long timeout)
{
	DeviceLocker::ScopedLock lock(_locker);
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Setting timeout %lu for sensor %s...",
			timeout, inv_sensor_2str(sensor));
	handleError(inv_device_set_sensor_timeout(_device, sensor, timeout), DEVICE_SETTING_CMD_DESC);
}

void DeviceClient::flushSensor(int sensor)
{
	DeviceLocker::ScopedLock lock(_locker);
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Flushing sensor %s...",
			inv_sensor_2str(sensor));
	handleError(inv_device_flush_sensor(_device, sensor), DEVICE_SETTING_CMD_DESC);
}

void DeviceClient::setSensorBias(int sensor, const float bias[3])
{
	DeviceLocker::ScopedLock lock(_locker);
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Setting bias { %f , %f, %f } for sensor %s...",
			bias[0], bias[1], bias[2], inv_sensor_2str(sensor));
	handleError(inv_device_set_sensor_bias(_device, sensor, bias), DEVICE_SETTING_CMD_DESC);
}

void DeviceClient::getSensorBias(int sensor, float bias[3])
{
	DeviceLocker::ScopedLock lock(_locker);
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Getting bias for sensor %s...",
			inv_sensor_2str(sensor));
	handleError(inv_device_get_sensor_bias(_device, sensor, bias), DEVICE_SETTING_CMD_DESC);
}

void DeviceClient::setSensorMountingMatrix(int sensor, const float matrix[9])
{
	DeviceLocker::ScopedLock lock(_locker);
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Setting matrix { %f , %f, %f ; %f , %f, %f ;"
			"%f , %f, %f } for sensor %s...",
			matrix[0], matrix[1], matrix[2],
			matrix[3], matrix[4], matrix[5],
			matrix[6], matrix[7], matrix[8],
			inv_sensor_2str(sensor));

	handleError(inv_device_set_sensor_mounting_matrix(_device, sensor, matrix), DEVICE_SETTING_CMD_DESC);
}

void DeviceClient::getSensorData(int sensor, inv_sensor_event_t & event)
{
	DeviceLocker::ScopedLock lock(_locker);
	handleError(inv_device_get_sensor_data(_device, sensor, &event), DEVICE_SETTING_CMD_DESC);
}

bool DeviceClient::selfTest(int sensor)
{
	return selfTestImpl(sensor);
}

bool DeviceClient::selfTestImpl(int sensor)
{
	DeviceLocker::ScopedLock lock(_locker);
	const int rc = inv_device_self_test(_device, sensor);

	/* just return false if self test has failed, do not throw exception */
	if(rc != INV_ERROR)
		handleError(rc, "Error during device self-test");

	return (rc == 0) ? true : false;
}

void DeviceClient::sensorEventCallback(const inv_sensor_event_t * event, void * arg)
{
	DeviceClient * self = reinterpret_cast<DeviceClient*>(arg);
	self->onSensorEvent(event);
}

void DeviceClient::onSensorEvent(const inv_sensor_event_t * event)
{
	// don't lock (we are in same context as poll() function, so it's already locked).
	if (_listener_ref)
		_listener_ref->notify(*event);
}

void DeviceClient::setListener(SensorEventsListener * listener)
{
	DeviceLocker::ScopedLock lock(_locker);
	_listener_ref = listener;
}

void DeviceClient::poll()
{
	DeviceLocker::ScopedLock lock(_locker);
	handleError(inv_device_poll(_device), "Error polling device");
}

bool DeviceClient::setSensorConfig(int sensor, const std::string & settings, const std::string & value)
{
	DeviceLocker::ScopedLock lock(_locker);
	return setSensorConfigImpl(sensor, settings, value);
}

bool DeviceClient::setSensorConfigCommon(int sensor, const std::string & settings, const std::string & value)
{
	INV_MSG(INV_MSG_LEVEL_DEBUG, "Setting sensor config: setting='%s' value='%s'", settings.c_str(), value.c_str());

	if (settings == "mmatrix" || settings == "mounting_matrix" || 
			settings == "matrix" || settings == "mounting-matrix") {
		// parse input string with a oldie scanf
		inv_sensor_config_mounting_mtx_t cfg;
		int rc = sscanf(value.c_str(), "%f %f %f %f %f %f %f %f %f",
				&cfg.matrix[0], &cfg.matrix[1], &cfg.matrix[2],
				&cfg.matrix[3], &cfg.matrix[4], &cfg.matrix[5],
				&cfg.matrix[6], &cfg.matrix[7], &cfg.matrix[8]);
		if(rc == 9) {
			rc = inv_device_set_sensor_config(_device, sensor, INV_SENSOR_CONFIG_MOUNTING_MATRIX, &cfg, sizeof(cfg));
			handleError(rc, "Fail to set mounting matrix");
		}
		else {
			handleError(INV_ERROR_BAD_ARG, "Invalid argument for 'mmatrix' setting");
		}
	}
	else if(settings == "fsr") {
		// parse input string with a oldie scanf
		unsigned fsr;
		int rc = sscanf(value.c_str(), "%u", &fsr);
		std::out << "ICIIIIIIIIIIIIII rc= " << rc  << " ---------------------------------" << std::endl;
		if(rc == 1) {
			inv_sensor_config_fsr_t cfg = { fsr };
			rc = inv_device_set_sensor_config(_device, sensor, INV_SENSOR_CONFIG_FSR, &cfg, sizeof(cfg));
			handleError(rc, "Fail to set FSR");
		}
		else {
			handleError(INV_ERROR_BAD_ARG, "Invalid argument for 'fsr' setting");
		}
	}
	else if(settings == "powermode") {
		// parse input string with a oldie scanf
		unsigned powermode;
		int rc = sscanf(value.c_str(), "%u", &powermode);

		if(rc == 1) {
			inv_sensor_config_powermode_t cfg  = { powermode };
			rc = inv_device_set_sensor_config(_device, sensor, INV_SENSOR_CONFIG_POWER_MODE, &cfg, sizeof(cfg));
			handleError(rc, "Fail to set power mode");
		}
		else {
			handleError(INV_ERROR_BAD_ARG, "Invalid argument for 'powermode' setting");
		}
	}
	else if(settings == "reset") {
		int rc = inv_device_set_sensor_config(_device, sensor, INV_SENSOR_CONFIG_RESET, 0, 0);
		handleError(rc, "Fail to set reset");
	}
	else if(settings == "gain" || settings == "soft_iron") {
		// parse input string with a oldie scanf
		inv_sensor_config_gain_t cfg;
		int rc = sscanf(value.c_str(), "%f %f %f %f %f %f %f %f %f",
				&cfg.gain[0], &cfg.gain[1], &cfg.gain[2],
				&cfg.gain[3], &cfg.gain[4], &cfg.gain[5],
				&cfg.gain[6], &cfg.gain[7], &cfg.gain[8]);

		if(rc == 9) {
			rc = inv_device_set_sensor_config(_device, sensor, INV_SENSOR_CONFIG_GAIN, &cfg, sizeof(cfg));
			handleError(rc, "Fail to set gain matrix");
		}
		else {
			handleError(INV_ERROR_BAD_ARG, "Invalid argument for 'gain' setting");
		}
	}
	else {
		return false;
	}

	return true;
}

bool DeviceClient::setSensorConfigImpl(int sensor, const std::string & settings, const std::string & value)
{
	return setSensorConfigCommon(sensor, settings, value);
}

bool DeviceClient::getSensorConfig(int sensor, const std::string & settings, std::string & value)
{
	DeviceLocker::ScopedLock lock(_locker);
	return getSensorConfigImpl(sensor, settings, value);
}

bool DeviceClient::getSensorConfigImpl(int, const std::string &, std::string &)
{
	return false;
}

void DeviceClient::writeMemsRegister(int sensor, unsigned int reg_addr, const uint8_t* data, unsigned size)
{
	DeviceLocker::ScopedLock lock(_locker);
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Setting mems register for sensor %s...",
			inv_sensor_2str(sensor));
	handleError(inv_device_write_mems_register(_device, sensor, reg_addr, data, size),
			"Error writing to MEMS register");
}

void DeviceClient::readMemsRegister(int sensor, unsigned int reg_addr, uint8_t* data, unsigned size)
{
	DeviceLocker::ScopedLock lock(_locker);
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Getting mems register for sensor %s...",
			inv_sensor_2str(sensor));
	handleError(inv_device_read_mems_register(_device, sensor, reg_addr, data, size),
			"Error while reading MEMS register");
}

//-----------------------------------------------------------------------------
// DeviceClient::FwImage
//-----------------------------------------------------------------------------

void DeviceClient::FwImage::readImageFromFile(const std::string & path)
{
	freeImage();

	if(!path.empty()) {
		/* look for ".txt" at the end of patch */
		const char * c = strrchr(path.c_str(), '.');

		/* text image */
		if(c && (strcmp(c, ".txt") == 0 || strcmp(c, ".inc") == 0 ||
				strcmp(c, ".cpp") == 0 || strcmp(c, ".c") == 0 || strcmp(c, ".h") == 0)) {
			_is_txt = true;
		}

		/* read image from disk */
		if(_is_txt) {
			_data = inv_dmp_image_converter_parse(path.c_str(), &_size);
		} else {
			_data = inv_binary_file_read(path.c_str(), &_size);
		}
	}

	if(!_data)
		handleError(INV_ERROR_FILE, "Cannot read image file from disk");
}

void DeviceClient::FwImage::freeImage()
{
	if(_data) {
		if(_is_txt) {
			inv_dmp_image_converter_free(_data);
		} else {
			inv_binary_file_free(_data);
		}
		_data = 0;
	}
}

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

#ifndef _IDD_CLIENT_DEVICE_H_
#define _IDD_CLIENT_DEVICE_H_

#include "Invn/InvExport.h"

#include "Invn/Devices/Device.h"

#include <string>
#include <memory>

class SensorEventsListener;
class HostAdapterClient;
class DeviceClient;
class DeviceDebuggerHook;

class INV_EXPORT DeviceDebuggerHook
{
public:
	virtual ~DeviceDebuggerHook() {}
	virtual void waitForDebugger(DeviceClient * device) = 0;
};

class INV_EXPORT DeviceErrorHandler
{
public:
	virtual ~DeviceErrorHandler() {}
	virtual void handleDeviceError(DeviceClient * device, const std::exception& e) = 0;
};

/// Listener to retrieve logging stream from device
class INV_EXPORT DeviceLogger
{
public:
	virtual ~DeviceLogger() { }

	/// Called when a character from device is received
	virtual void logFromDevice(DeviceClient * device, int c) = 0;
};

class INV_EXPORT DeviceLocker
{
public:
	DeviceLocker();
	virtual ~DeviceLocker();
	virtual void lock();
	virtual void unlock();

	struct ScopedLock
	{
		ScopedLock(DeviceLocker* locker);
		~ScopedLock();
	private:
		DeviceLocker* _locker;
	};

private:
	struct Impl;
	Impl * _impl;
};

class INV_EXPORT DeviceClient
{
public:
	DeviceClient(HostAdapterClient* serif, inv_device_t * dev);
	virtual ~DeviceClient();

	virtual void setup() = 0;
	virtual void cleanup() = 0;

	/// Return handle to underlying inv_device_t
	inv_device_t * getDevice() { return _device; }

	/// Change the image file used to flash the device.
	/// @param image_type type of the image (see children classes to know allowed types).
	/// @param path path to the image file
	/// @throw std::exception if the image type is not handled.
	virtual void setImage(int image_type, const std::string & path) { (void)image_type, (void)path; }

	/// Register a hook called inside device setup
	/// Hook can stop the thread, to let enough time to attach a debugger
	/// at a early stage
	/// @param debug_hook pointer to DebuggerHoook object
	virtual void registerDebuggerHook(DeviceDebuggerHook * dbg_hook);

	virtual void watchdogPoll() {}

	virtual void dumpFlash(unsigned char *buffer) {}

	/// Helper method to call debugger hook
	void callDebuggerHook();

	/// Register a listener that will receive the sensor events
	void setListener(SensorEventsListener * listener);

	int whoami();
	void getFwInfo(inv_fw_version_t & version);
	void setRunningState(bool state);
	bool pingSensor(int sensor);
	void startSensor(int sensor);
	void stopSensor(int sensor);
	void setSensorPeriod(int sensor, unsigned long period);
	void setSensorPeriodUs(int sensor, unsigned long period_us);
	void setSensorTimeout(int sensor, unsigned long timeout);
	void flushSensor(int sensor);
	void setSensorBias(int sensor, const float bias[3]);
	void getSensorBias(int sensor, float bias[3]);
	void setSensorMountingMatrix(int sensor, const float matrix[9]);
	void getSensorData(int sensor, inv_sensor_event_t & event);

	/// Configure a sensor
	/// Refer to concrete implementation for supported settings name/value
	/// @param sensor   sensor to configure
	/// @param settings settings name
	/// @param value    settings value
	/// @return true if settings was successfuly applied, false if ignored
	bool setSensorConfig(int sensor, const std::string & settings,
			const std::string & value);

	/// Retrieve sensor configuration
	/// Refer to concrete implementation for supported settings name/value
	/// @param sensor   sensor to get configuration from
	/// @param settings settings name
	/// @param value    settings value
	/// @return true if settings was successfuly returned, false if otherwise
	bool getSensorConfig(int sensor, const std::string & settings,  std::string & data);

	/// Perform self-test procedure for MEMS component of the device
	/// @param sensor sensor id corresponding to a mems
	/// @return true if self-test was successful, false otherwise
	bool selfTest(int sensor);

	/// Write to mems register
	/// @param sensor    hw sensor
	/// @param reg_addr  mems register (expected to be a 8bits value
	///                                 if > 8bits, MSByte will designate the bank)
	/// @param data      data to write
	/// @param size      length of data to write
	void writeMemsRegister(int sensor, unsigned int reg_addr, const uint8_t* data, unsigned size);

	/// Read from mems register
	/// @param sensor    hw sensor
	/// @param reg_addr  mems register (expected to be a 8bits value
	///                                 if > 8bits, MSByte will designate the bank)
	/// @param data      output buffer
	/// @param size      length of data to read
	void readMemsRegister(int sensor, unsigned int reg_addr, uint8_t* data, unsigned size);

	/// Set an optional locker in order to ensure this object is thread-safe
	void setLocker(DeviceLocker* locker) { _locker = locker; }
	DeviceLocker* getLocker() const { return _locker; }

	/// Read events from the FIFO of the connected device and bufferize them.
	/// @see popEvent() in order to retrieve the event
	void poll();

	/// Update internal listener context
	/// This should be used only if you know what you're doing
	/// FIXME this is a ugly hack to make DeviceClient listener work when usgin proxy
	void updateInternalSensorListenerContext(void * new_context)
	{
		_listener.context = new_context;
	}

	/// Small class to read FW image from disc according to its name
	class FwImage {
	public:
		FwImage()
			: _is_txt(false), _data(0), _size(0)
		{
		}

		FwImage(const std::string & path)
			: _is_txt(false), _data(0), _size(0)
		{
			readImageFromFile(path);
		}

		~FwImage()
		{
			freeImage();
		}

		/// Read image located to path in memory and fill ImageInfo accordingly
		void readImageFromFile(const std::string & path);

		/// Release image from memory
		void freeImage();

		const uint8_t * data() const
		{
			return _data;
		}

		unsigned long size() const
		{
			return _size;
		}
	private:
		bool          _is_txt;
		uint8_t *     _data;
		unsigned long _size;
	};

protected:
	static void handleError(int rc, const char * desc = "");

	/// Handle common config
	/// If setSensorConfigImpl() is defined on the derived class, it is up to the implementation to call it
	/// Allowed settings supported so far:
	///  - mmatrix, mounting_matrix => INV_SENSOR_CONFIG_MOUNTING_MATRIX
	///  - fsr                      => INV_SENSOR_CONFIG_FSR
	///  - reset                    => INV_SENSOR_CONFIG_RESET
	virtual bool setSensorConfigCommon(int, const std::string &, const std::string &);

	/// Default implementation for setSensorConfig
	/// Currently calls setSensorConfigCommon()
	virtual bool setSensorConfigImpl(int, const std::string &, const std::string &);

	virtual bool getSensorConfigImpl(int, const std::string &, std::string &);

	virtual bool selfTestImpl(int);

private:
	static void sensorEventCallback(const inv_sensor_event_t * event, void * arg);
	void onSensorEvent(const inv_sensor_event_t * event);

protected:
	HostAdapterClient*    _serif;
	inv_device_t *        _device;
	inv_sensor_listener_t _listener;
	DeviceDebuggerHook *  _debugger_hook;
	mutable DeviceLocker* _locker;

private:
	SensorEventsListener* _listener_ref;
};

#endif /* _IDD_CLIENT_DEVICE_H_ */

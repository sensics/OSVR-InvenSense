//-----------------------------------------------------------------------------
/*
 Copyright © 2016 InvenSense Inc. All rights reserved.

 This software, related documentation and any modifications thereto collectively “Software” is subject
 to InvenSense and its licensors' intellectual property rights under U.S. and international copyright and
 other intellectual property rights laws.

 InvenSense and its licensors retain all intellectual property and proprietary rights in and to the Software
 and any use, reproduction, disclosure or distribution of the Software without an express license
 agreement from InvenSense is strictly prohibited.
 */
//-----------------------------------------------------------------------------

#ifndef _IDD_DevicePoller_h_
#define _IDD_DevicePoller_h_

#include "Invn/InvExport.h"

// STL
#include <memory>

// fwd declr
class DeviceClient;
class DeviceErrorHandler;

//-----------------------------------------------------------------------------

/// Manage a thread that will poll periodically a device in order to retrieve data events.
/// When using a poller, it's important that the device uses a locker (see DeviceClient::setLocker()).
class INV_EXPORT DevicePoller
{
public:
	/// Scoped object that will stop/re-start a given data event poller.
	/// The poller will be restarted at the end of the scope life of this object
	/// (only if it was arlready started).
	struct ScopedStopper
	{
		ScopedStopper(DevicePoller& looper);
		~ScopedStopper();
		DevicePoller& _looper;
		bool _bStarted;
	};

	/// Probe object in order to monitor sleeping times of the poller.
	struct ProbeListener
	{
		/// Called just after polling device and before the poller will sleep.
		/// Never called if period is 0.
		/// @param iPollingTime  spent time (in us) to poll the device
		/// @param iSleepTime    sleeping time (in us) of the thread before next poll
		virtual void notifySleepTime(unsigned long long iPollingTime, unsigned long long iSleepTime) = 0;
	};

	/// Constructor
	/// @param device reference to a device to poll (optional)
	DevicePoller(DeviceClient* device = 0);
	virtual ~DevicePoller();

	/// Change the device to poll
	void setDevice(DeviceClient* device);

	/// Retrieve which device is currently polled
	DeviceClient* getDevice() const;

	/// Start polling thread
	/// @throw std::runtime_error if the device doesn't have a locker
	void start();

	/// Stop the polling thread (if not stopped yet)
	void stop();

	/// Indicate if the polling thread is started
	bool isStarted() const;

	/// Allow to change the period of the poller thread
	/// @param period period in ms of the poller thread (default is 5ms).
	///               if period is 0, thread will never sleep
	void setPeriod(unsigned int period);

	/// Register a handler called upon device polling error
	/// Called from poller thread
	/// @param handler pointer to DeviceErrorHandler object
	void registerErrorHandler(DeviceErrorHandler* handler);

	/// Retrieve the error handler
	DeviceErrorHandler* getErrorHandler() const;

	/// Register a probe
	void registerProbe(ProbeListener* probe);

	/// Will be called periodically by the managed thread
	virtual void process() = 0; 

private:
	struct Impl;

	// disable annoying warnings related to usage of STL templates in exported class
	Impl * _impl;
};

//-----------------------------------------------------------------------------

#endif // _IDD_DevicePoller_h_

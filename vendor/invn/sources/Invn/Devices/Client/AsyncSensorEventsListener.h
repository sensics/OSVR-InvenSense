//-----------------------------------------------------------------------------
/*
 Copyright © 2015 InvenSense Inc. All rights reserved.

 This software, related documentation and any modifications thereto collectively “Software” is subject
 to InvenSense and its licensors' intellectual property rights under U.S. and international copyright and
 other intellectual property rights laws.

 InvenSense and its licensors retain all intellectual property and proprietary rights in and to the Software
 and any use, reproduction, disclosure or distribution of the Software without an express license
 agreement from InvenSense is strictly prohibited.
 */
//-----------------------------------------------------------------------------

#ifndef _IDD_AsyncSensorEventsListener_h_
#define _IDD_AsyncSensorEventsListener_h_

#include "Invn/InvExport.h"

#include "Invn/Devices/Client/SensorEventsListener.h"

// STL
#include <memory>

// fwd declr
class DeviceClient;

//-----------------------------------------------------------------------------

/// Consume data event and notify a given listener from a dedicated thread.
/// This allows to make quicker the polling actions as listener process will be in another thread.
/// A queue of data events is managed internally.
class INV_EXPORT AsyncSensorEventsListener : public SensorEventsListener
{
public:
	/// Scoped object that will stop/re-start a given asynchronous listener.
	/// The poller will be restarted at the end of the scope life of this object
	/// (only if it was arlready started).
	struct ScopedStopper
	{
		ScopedStopper(AsyncSensorEventsListener& listener);
		~ScopedStopper();
		AsyncSensorEventsListener& _listener;
		bool _bStarted;
	};

	/// Probe object in order to monitor the buffer state.
	/// For example, if sensor event rate is too quick and sensor event listener process is too slow,
	/// the buffer could saturate. You can diagnostic that thanks to this object.
	struct ProbeListener
	{
		/// Called when an event is pushed to the buffer
		virtual void notifyPushEvent(int iBufferSize) = 0;

		/// Called when an event is pop from the buffer
		virtual void notifyPopEvent(int iBufferSize) = 0;
	};

	/// Constructor
	/// @param bufferSize	Maximum of events of the internal buffer
	AsyncSensorEventsListener(unsigned int bufferSize = 4096);

	/// Desctructor.
	/// Will automatically stop the internal thread.
	virtual ~AsyncSensorEventsListener();

	/// Register a listener to receive sensor events
	void setListener(SensorEventsListener * listener);

	/// Start the asynchronous consumer of sensor events
	void start();

	/// Stop the thread
	void stop();

	/// Indicate if the asynchronous consumer is started
	bool isStarted() const;

	/// Register a probe
	void registerProbe(ProbeListener* probe);

protected:
	virtual void notify(const inv_sensor_event_t & event);

private:
	struct Impl;
	Impl * _impl;
};

//-----------------------------------------------------------------------------

#endif // _IDD_AsyncSensorEventsListener_h_

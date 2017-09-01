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

#include "AsyncSensorEventsListener.h"

#include <queue>

// Poco
#include "Poco/Activity.h"
#include "Poco/Mutex.h"
#include "Poco/Semaphore.h"

//-----------------------------------------------------------------------------

struct AsyncSensorEventsListener::Impl
{
	Impl(unsigned int bufferSize)
	: bufferSize(bufferSize)
	, listener(0)
	, probe(0)
	, activity(this, &Impl::runActivity)
	, readyEvents(0,bufferSize)
	{

	}

	void runActivity()
	{
		inv_sensor_event_t e;
		for (;;)
		{
			// wait until an event is available
			readyEvents.wait();
			if (activity.isStopped())
				return;

			// pop event
			{
				Poco::FastMutex::ScopedLock lock(mtx);
				if (events.empty())
					continue;

				if (probe)
					probe->notifyPopEvent(events.size());
				e = events.front();
				events.pop();
			}

			// dispatch event
			listener->notify(e);
		}
	}

	unsigned int bufferSize;
	SensorEventsListener* listener;
	ProbeListener* probe;
	Poco::Activity<AsyncSensorEventsListener::Impl> activity;
	std::queue<inv_sensor_event_t> events;
	Poco::Semaphore readyEvents;
	Poco::FastMutex mtx;
};

//-----------------------------------------------------------------------------

AsyncSensorEventsListener::ScopedStopper::ScopedStopper(AsyncSensorEventsListener& listener)
: _listener(listener)
, _bStarted(listener.isStarted())
{
	listener.stop();
}

AsyncSensorEventsListener::ScopedStopper::~ScopedStopper()
{
	if (_bStarted)
		_listener.start();
}

//-----------------------------------------------------------------------------

AsyncSensorEventsListener::AsyncSensorEventsListener(unsigned int bufferSize)
: _impl(new Impl(bufferSize))
{

}

//-----------------------------------------------------------------------------

AsyncSensorEventsListener::~AsyncSensorEventsListener()
{
	stop();
	delete _impl;
}

//-----------------------------------------------------------------------------

void AsyncSensorEventsListener::notify(const inv_sensor_event_t & e)
{
	Poco::FastMutex::ScopedLock lock(_impl->mtx);

	if (_impl->probe)
		_impl->probe->notifyPushEvent(_impl->events.size());
	if (_impl->events.size() < _impl->bufferSize)
	{
		_impl->events.push(e);
		_impl->readyEvents.set();
	}
}

//-----------------------------------------------------------------------------

void AsyncSensorEventsListener::setListener(SensorEventsListener * listener)
{
	ScopedStopper lock(*this);
	_impl->listener = listener;
}

//-----------------------------------------------------------------------------

void AsyncSensorEventsListener::start()
{
	if (_impl->listener)
		_impl->activity.start();
}

//-----------------------------------------------------------------------------

void AsyncSensorEventsListener::stop()
{
	if (_impl->activity.isRunning())
	{
		_impl->activity.stop();
		_impl->readyEvents.set(); // awake the thread
		_impl->activity.wait();
	}
}

//-----------------------------------------------------------------------------

bool AsyncSensorEventsListener::isStarted() const
{
	return _impl->activity.isRunning();
}

//-----------------------------------------------------------------------------

void AsyncSensorEventsListener::registerProbe(ProbeListener* probe)
{
	Poco::FastMutex::ScopedLock lock(_impl->mtx);
	_impl->probe = probe;
}

//-----------------------------------------------------------------------------


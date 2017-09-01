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

#include "DevicePoller.h"
#include "DeviceClient.h"

#include "Invn/EmbUtils/Message.h"

// Poco
#include "Poco/Clock.h"
#include "Poco/Event.h"
#include "Poco/Thread.h"

//-----------------------------------------------------------------------------

struct INV_EXPORT DevicePoller::Impl : public Poco::Runnable
{
	Impl(DevicePoller* poller)
		: poll(poller)
		, device(0)
		, errorHandler(0)
		, probe(0)
		, period(5000)
		, started(false)
	{
	}

	virtual ~Impl() { }

	virtual void run()
	{
		Poco::Clock time;

		while (started)
		{
			// process periodic poll
			poll->process();

			// compute polling/sleeping time
			if (period > 0)
			{
				const Poco::Clock::ClockDiff iSpentTime = time.elapsed();
				const Poco::Clock::ClockDiff iSleep = iSpentTime < period ? period - iSpentTime : 0;
				if (probe)
					probe->notifySleepTime(iSpentTime, iSleep);

				// wait & check if we must stop
				if (wakeUp.tryWait(long(iSleep/1000)) && !started)
					return;
				time.update();
			}
			// just compute polling time (for probe)
			else if (probe)
			{
				Poco::Clock now;
				probe->notifySleepTime(now-time, 0);
				time = now;
			}
		}
	}

	DevicePoller* poll;
	DeviceClient* device;
	DeviceErrorHandler* errorHandler;
	ProbeListener* probe;
	int period;
	Poco::Thread looper;
	Poco::Event wakeUp;
	bool started;
};

//-----------------------------------------------------------------------------

DevicePoller::ScopedStopper::ScopedStopper(DevicePoller& looper)
: _looper(looper)
, _bStarted(looper.isStarted())
{
	looper.stop();
}

DevicePoller::ScopedStopper::~ScopedStopper()
{
	if (_bStarted)
		_looper.start();
}

//-----------------------------------------------------------------------------

DevicePoller::DevicePoller(DeviceClient* device)
: _impl(new Impl(this))
{
	setDevice(device);
}

//-----------------------------------------------------------------------------

DevicePoller::~DevicePoller()
{
	stop();
	delete _impl;
}

//-----------------------------------------------------------------------------

void DevicePoller::setPeriod(unsigned int period)
{
	ScopedStopper lock(*this);
	_impl->period = period*1000;
}

//-----------------------------------------------------------------------------

void DevicePoller::registerErrorHandler(DeviceErrorHandler* handler)
{
	ScopedStopper lock(*this);
	_impl->errorHandler = handler;
}

//-----------------------------------------------------------------------------

DeviceErrorHandler* DevicePoller::getErrorHandler() const
{
	return _impl->errorHandler;
}

//-----------------------------------------------------------------------------

void DevicePoller::setDevice(DeviceClient* device)
{
	if (device == this->_impl->device)
		return;

	ScopedStopper lock(*this);
	_impl->device = device;
}

//-----------------------------------------------------------------------------

DeviceClient* DevicePoller::getDevice() const
{
	return _impl->device;
}

//-----------------------------------------------------------------------------

void DevicePoller::start()
{
	if (_impl->device && !_impl->looper.isRunning())
	{
		// check if device has a locker
		if (!_impl->device->getLocker())
			throw std::runtime_error("Cannot start the poller: missing device locker");
		_impl->started = true;
		_impl->wakeUp.reset();
		_impl->looper.start(*_impl);
	}
}

//-----------------------------------------------------------------------------

void DevicePoller::stop()
{
	if (_impl->started)
	{
		_impl->started = false;
		_impl->wakeUp.set();
		_impl->looper.join();
	}
}

//-----------------------------------------------------------------------------

bool DevicePoller::isStarted() const
{
	return _impl->started;
}

//-----------------------------------------------------------------------------

void DevicePoller::registerProbe(ProbeListener* probe)
{
	ScopedStopper lock(*this);
	_impl->probe = probe;
}

//-----------------------------------------------------------------------------


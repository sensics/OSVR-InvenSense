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

#include "WatchdogPoller.h"
#include "DeviceClient.h"

#include "Invn/EmbUtils/Message.h"

//-----------------------------------------------------------------------------

WatchdogPoller::WatchdogPoller(DeviceClient* device)
: DevicePoller(device)
{
	setPeriod(2500); // default period of 2.5s
}

//-----------------------------------------------------------------------------

WatchdogPoller::~WatchdogPoller()
{
	stop();
}

//-----------------------------------------------------------------------------

void WatchdogPoller::process()
{
	try
	{
		getDevice()->watchdogPoll();
	}
	catch (const std::exception& e)
	{
		if (getErrorHandler())
			getErrorHandler()->handleDeviceError(getDevice(), e);
		else
			INV_MSG(INV_MSG_LEVEL_ERROR, "Watchdog error: %s", e.what());
	}
}

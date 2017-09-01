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

#include "DataEventPoller.h"
#include "DeviceClient.h"

#include "Invn/EmbUtils/Message.h"

//-----------------------------------------------------------------------------

DataEventPoller::DataEventPoller(DeviceClient* device)
: DevicePoller(device)
{
	setPeriod(5); // default period of 5ms
}

//-----------------------------------------------------------------------------

DataEventPoller::~DataEventPoller()
{
	stop();
}

//-----------------------------------------------------------------------------

void DataEventPoller::process()
{
	try
	{
		getDevice()->poll();
	}
	catch (const std::exception& e)
	{
		if (getErrorHandler())
			getErrorHandler()->handleDeviceError(getDevice(), e);
		else
			INV_MSG(INV_MSG_LEVEL_ERROR, "Polling device error: %s", e.what());
	}
}

//-----------------------------------------------------------------------------

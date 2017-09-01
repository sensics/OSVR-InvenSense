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

#ifndef _IDD_DataEventPoller_h_
#define _IDD_DataEventPoller_h_

#include "Invn/InvExport.h"

#include "DevicePoller.h"

// fwd declr
class DeviceClient;

//-----------------------------------------------------------------------------

/// Manage a thread that will poll periodically a device in order to retrieve data events.
/// When using a poller, it's important that the device uses a locker (see DeviceClient::setLocker()).
class INV_EXPORT DataEventPoller : public DevicePoller
{
public:
	/// Constructor
	/// @param device reference to a device to poll (optional)
	DataEventPoller(DeviceClient* device = 0);
	~DataEventPoller();

	/// Will be called periodically by the managed thread
	/// The actual polling is done here
	void process();
};

//-----------------------------------------------------------------------------

#endif // _IDD_DataEventPoller_h_

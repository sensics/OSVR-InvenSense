/*
 * ________________________________________________________________________________________________________
 * Copyright © 2015 InvenSense Inc.  All rights reserved.
 *
 * This software and/or documentation  (collectively “Software”) is subject to InvenSense intellectual property rights
 * under U.S. and international copyright and other intellectual property rights laws.
 *
 * The Software contained herein is PROPRIETARY and CONFIDENTIAL to InvenSense and is provided
 * solely under the terms and conditions of a form of InvenSense software license agreement between
 * InvenSense and you and any use, modification, reproduction or disclosure of the Software without
 * such agreement or the express written consent of InvenSense is strictly prohibited.
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

#ifndef _SENSOR_EVENTS_DISPATCHER_H_
#define _SENSOR_EVENTS_DISPATCHER_H_

#include "Invn/InvExport.h"

#include "Invn/Devices/Client/SensorEventsListener.h"
#include "Invn/HostUtils/Os.h"

#include <list>

class INV_EXPORT SensorEventsDispatcher : public SensorEventsListener
{
public:
	SensorEventsDispatcher();
	~SensorEventsDispatcher();
	void subscribe(SensorEventsListener* listener);
	void unsubscribe(SensorEventsListener* listener);

protected:
	void notify(const inv_sensor_event_t & event);

private:
	typedef std::list<SensorEventsListener *> List;

	inv_os_mutex_t _list_mutex;

	// disable annoying warnings related to usage of STL templates in exported class
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251)
#endif
	List           _list;
#ifdef _MSC_VER
#pragma warning( pop )
#endif
};

#endif /* _SENSOR_EVENTS_DISPATCHER_H_*/

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

#include "SensorEventsDispatcher.h"

#include <stdexcept>
#include <algorithm>

SensorEventsDispatcher::SensorEventsDispatcher()
{
	inv_os_mutex_create(&_list_mutex);
}

SensorEventsDispatcher::~SensorEventsDispatcher()
{
	inv_os_mutex_destroy(_list_mutex);
}

void SensorEventsDispatcher::subscribe(SensorEventsListener* listener)
{
	inv_os_mutex_lock(_list_mutex, -1);

	if (std::find(_list.begin(), _list.end(), listener) == _list.end())
		_list.push_back(listener);

	inv_os_mutex_unlock(_list_mutex);
}

void SensorEventsDispatcher::unsubscribe(SensorEventsListener* listener)
{
	inv_os_mutex_lock(_list_mutex, -1);
	_list.remove(listener);
	inv_os_mutex_unlock(_list_mutex);
}

void SensorEventsDispatcher::notify(const inv_sensor_event_t & event)
{
	inv_os_mutex_lock(_list_mutex, -1);

	for(List::const_iterator it=_list.begin(); it != _list.end(); ++it)
		(*it)->notify(event);

	inv_os_mutex_unlock(_list_mutex);
}

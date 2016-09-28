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

#ifndef _SENSOR_EVENTS_PRINTER_H_
#define _SENSOR_EVENTS_PRINTER_H_

#include "Invn/Devices/SensorTypes.h"
#include "Invn/HostUtils/BinaryDecoder.h"

#include <cassert>
#include <cstdio>
#include <map>

#include <Poco/Mutex.h>

class SensorEventsPrinter
{
public:
	SensorEventsPrinter()
	{
		assert(!_instance);
		_instance = this;
	}

	~SensorEventsPrinter()
	{
		_instance = 0;
	}

	static SensorEventsPrinter & instance()
	{
		assert(_instance);
		return *_instance;
	}

	bool configure(int sensor, const std::string & desc);
	void print(const inv_sensor_event_t & event, FILE * stream);
	static void printDefault(const inv_sensor_event_t & event,
			FILE * stream = stdout);
	static const char * activityName(int act);
	static const char * activityExtName(int act);

private:
	std::map<int,BinaryDecoder>  _decoders;
	Poco::Mutex                  _decoders_mutex;
	static SensorEventsPrinter * _instance;
};

#endif /* _SENSOR_EVENTS_PRINTER_H_ */

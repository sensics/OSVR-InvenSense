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

#include "CommandSensor.h"

#include "Invn/Devices/Client/DeviceClient.h"
#include "Invn/Devices/Client/DataEventPoller.h"
#include "DeviceInstance.h"
#include "SensorEventsPrinter.h"

#include <Poco/Poco.h>
#include <Poco/String.h>
#include <Poco/StringTokenizer.h>
#include <Poco/NumberParser.h>

#include <cstring>
#include <cstdio>

static const struct {
	const char * sname;
	unsigned int id;
} sensor_sname_map[] = {
	{ "all" ,      INV_SENSOR_TYPE_MAX },
	{ "wall" ,     INV_SENSOR_TYPE_MAX | INV_SENSOR_TYPE_WU_FLAG },
	{ "acc" ,      INV_SENSOR_TYPE_ACCELEROMETER },
	{ "wacc" ,     INV_SENSOR_TYPE_ACCELEROMETER | INV_SENSOR_TYPE_WU_FLAG },
	{ "atemp",     INV_SENSOR_TYPE_AMBIENT_TEMPERATURE },
	{ "watemp",    INV_SENSOR_TYPE_AMBIENT_TEMPERATURE | INV_SENSOR_TYPE_WU_FLAG },
	{ "axis",      INV_SENSOR_TYPE_3AXIS },
	{ "waxis",     INV_SENSOR_TYPE_3AXIS | INV_SENSOR_TYPE_WU_FLAG },
	{ "b2s",       INV_SENSOR_TYPE_B2S },
	{ "wb2s",      INV_SENSOR_TYPE_B2S | INV_SENSOR_TYPE_WU_FLAG },
	{ "bac",       INV_SENSOR_TYPE_BAC },
	{ "wbac",      INV_SENSOR_TYPE_BAC | INV_SENSOR_TYPE_WU_FLAG },
	{ "georv",     INV_SENSOR_TYPE_GEOMAG_ROTATION_VECTOR },
	{ "wgeorv",    INV_SENSOR_TYPE_GEOMAG_ROTATION_VECTOR | INV_SENSOR_TYPE_WU_FLAG },
	{ "glance",    INV_SENSOR_TYPE_GLANCE_GESTURE },
	{ "wglance",   INV_SENSOR_TYPE_GLANCE_GESTURE | INV_SENSOR_TYPE_WU_FLAG },
	{ "gra",       INV_SENSOR_TYPE_GRAVITY },
	{ "wgra",      INV_SENSOR_TYPE_GRAVITY | INV_SENSOR_TYPE_WU_FLAG },
	{ "grv",       INV_SENSOR_TYPE_GAME_ROTATION_VECTOR },
	{ "wgrv",      INV_SENSOR_TYPE_GAME_ROTATION_VECTOR | INV_SENSOR_TYPE_WU_FLAG },
	{ "gyr",       INV_SENSOR_TYPE_GYROSCOPE },
	{ "wgyr",      INV_SENSOR_TYPE_GYROSCOPE | INV_SENSOR_TYPE_WU_FLAG },
	{ "hrm",       INV_SENSOR_TYPE_HEART_RATE },
	{ "whrm",      INV_SENSOR_TYPE_HEART_RATE | INV_SENSOR_TYPE_WU_FLAG },
	{ "humidity",  INV_SENSOR_TYPE_HUMIDITY },
	{ "whumidity", INV_SENSOR_TYPE_HUMIDITY  | INV_SENSOR_TYPE_WU_FLAG },
	{ "light",     INV_SENSOR_TYPE_LIGHT },
	{ "wlight",    INV_SENSOR_TYPE_LIGHT | INV_SENSOR_TYPE_WU_FLAG },
	{ "linacc",    INV_SENSOR_TYPE_LINEAR_ACCELERATION },
	{ "wlinacc",   INV_SENSOR_TYPE_LINEAR_ACCELERATION | INV_SENSOR_TYPE_WU_FLAG },
	{ "mag",       INV_SENSOR_TYPE_MAGNETOMETER },
	{ "wmag",      INV_SENSOR_TYPE_MAGNETOMETER | INV_SENSOR_TYPE_WU_FLAG },
	{ "reserved",  INV_SENSOR_TYPE_RESERVED },
	{ "wreserved", INV_SENSOR_TYPE_RESERVED | INV_SENSOR_TYPE_WU_FLAG },
	{ "ori",       INV_SENSOR_TYPE_ORIENTATION },
	{ "wori",      INV_SENSOR_TYPE_ORIENTATION | INV_SENSOR_TYPE_WU_FLAG },
	{ "pdr",       INV_SENSOR_TYPE_PDR },
	{ "wpdr",      INV_SENSOR_TYPE_PDR | INV_SENSOR_TYPE_WU_FLAG },
	{ "pickup",    INV_SENSOR_TYPE_PICK_UP_GESTURE },
	{ "wpickup",   INV_SENSOR_TYPE_PICK_UP_GESTURE | INV_SENSOR_TYPE_WU_FLAG },
	{ "pres",      INV_SENSOR_TYPE_PRESSURE },
	{ "wpres",     INV_SENSOR_TYPE_PRESSURE | INV_SENSOR_TYPE_WU_FLAG },
	{ "prox",      INV_SENSOR_TYPE_PROXIMITY },
	{ "wprox",     INV_SENSOR_TYPE_PROXIMITY | INV_SENSOR_TYPE_WU_FLAG },
	{ "rv",        INV_SENSOR_TYPE_ROTATION_VECTOR },
	{ "wrv",       INV_SENSOR_TYPE_ROTATION_VECTOR | INV_SENSOR_TYPE_WU_FLAG },
	{ "smd",       INV_SENSOR_TYPE_SMD },
	{ "wsmd",      INV_SENSOR_TYPE_SMD | INV_SENSOR_TYPE_WU_FLAG },
	{ "stepc",     INV_SENSOR_TYPE_STEP_COUNTER },
	{ "wstepc",    INV_SENSOR_TYPE_STEP_COUNTER | INV_SENSOR_TYPE_WU_FLAG },
	{ "stepd",     INV_SENSOR_TYPE_STEP_DETECTOR },
	{ "wstepd",    INV_SENSOR_TYPE_STEP_DETECTOR | INV_SENSOR_TYPE_WU_FLAG },
	{ "temp",      INV_SENSOR_TYPE_TEMPERATURE },
	{ "wtemp",     INV_SENSOR_TYPE_TEMPERATURE | INV_SENSOR_TYPE_WU_FLAG },
	{ "tilt",      INV_SENSOR_TYPE_TILT_DETECTOR },
	{ "wtilt",     INV_SENSOR_TYPE_TILT_DETECTOR | INV_SENSOR_TYPE_WU_FLAG },
	{ "ugyr",      INV_SENSOR_TYPE_UNCAL_GYROSCOPE },
	{ "wugyr",     INV_SENSOR_TYPE_UNCAL_GYROSCOPE | INV_SENSOR_TYPE_WU_FLAG },
	{ "umag",      INV_SENSOR_TYPE_UNCAL_MAGNETOMETER },
	{ "wumag",     INV_SENSOR_TYPE_UNCAL_MAGNETOMETER | INV_SENSOR_TYPE_WU_FLAG },
	{ "wake",      INV_SENSOR_TYPE_WAKE_GESTURE },
	{ "wwake",     INV_SENSOR_TYPE_WAKE_GESTURE | INV_SENSOR_TYPE_WU_FLAG },
	{ "eis",       INV_SENSOR_TYPE_EIS },
	{ "weis",      INV_SENSOR_TYPE_EIS | INV_SENSOR_TYPE_WU_FLAG },
	{ "ois",       INV_SENSOR_TYPE_OIS },
	{ "wois",      INV_SENSOR_TYPE_OIS | INV_SENSOR_TYPE_WU_FLAG },
	{ "racc",      INV_SENSOR_TYPE_RAW_ACCELEROMETER },
	{ "wracc",     INV_SENSOR_TYPE_RAW_ACCELEROMETER | INV_SENSOR_TYPE_WU_FLAG },
	{ "rgyr",      INV_SENSOR_TYPE_RAW_GYROSCOPE },
	{ "wrgyr",     INV_SENSOR_TYPE_RAW_GYROSCOPE | INV_SENSOR_TYPE_WU_FLAG },
	{ "rmag",      INV_SENSOR_TYPE_RAW_MAGNETOMETER },
	{ "wrmag",     INV_SENSOR_TYPE_RAW_MAGNETOMETER | INV_SENSOR_TYPE_WU_FLAG },
	{ "rtemp",     INV_SENSOR_TYPE_RAW_TEMPERATURE },
	{ "wrtemp",    INV_SENSOR_TYPE_RAW_TEMPERATURE | INV_SENSOR_TYPE_WU_FLAG },
	{ "kwr",       INV_SENSOR_TYPE_KWR },
	{ "wkwr",      INV_SENSOR_TYPE_KWR | INV_SENSOR_TYPE_WU_FLAG },
	{ "tsimu",     INV_SENSOR_TYPE_TSIMU },
	{ "wtsimu",    INV_SENSOR_TYPE_TSIMU | INV_SENSOR_TYPE_WU_FLAG },
	{ "rppg",      INV_SENSOR_TYPE_RAW_PPG },
	{ "wrppg",     INV_SENSOR_TYPE_RAW_PPG | INV_SENSOR_TYPE_WU_FLAG },
	{ "hrv",       INV_SENSOR_TYPE_HRV },
	{ "whrv",      INV_SENSOR_TYPE_HRV | INV_SENSOR_TYPE_WU_FLAG },
	{ "sleep",     INV_SENSOR_TYPE_SLEEP_ANALYSIS },
	{ "wsleep",    INV_SENSOR_TYPE_SLEEP_ANALYSIS | INV_SENSOR_TYPE_WU_FLAG },
	{ "bacext",    INV_SENSOR_TYPE_BAC_EXTENDED },
	{ "wbacext",   INV_SENSOR_TYPE_BAC_EXTENDED | INV_SENSOR_TYPE_WU_FLAG },
	{ "bacstat",   INV_SENSOR_TYPE_BAC_STATISTICS },
	{ "wbacstat",  INV_SENSOR_TYPE_BAC_STATISTICS | INV_SENSOR_TYPE_WU_FLAG },	
	{ "floorcl",   INV_SENSOR_TYPE_FLOOR_CLIMB_COUNTER },
	{ "wfloorcl",  INV_SENSOR_TYPE_FLOOR_CLIMB_COUNTER | INV_SENSOR_TYPE_WU_FLAG },
	{ "energy",    INV_SENSOR_TYPE_ENERGY_EXPENDITURE },
	{ "wenergy",   INV_SENSOR_TYPE_ENERGY_EXPENDITURE | INV_SENSOR_TYPE_WU_FLAG },
	{ "dist",      INV_SENSOR_TYPE_DISTANCE },
	{ "wdist",     INV_SENSOR_TYPE_DISTANCE | INV_SENSOR_TYPE_WU_FLAG },
	{ "shake",     INV_SENSOR_TYPE_SHAKE },
	{ "wshake",    INV_SENSOR_TYPE_SHAKE | INV_SENSOR_TYPE_WU_FLAG },
	{ "doubletap", INV_SENSOR_TYPE_DOUBLE_TAP },
	{ "wdoubletap",INV_SENSOR_TYPE_DOUBLE_TAP | INV_SENSOR_TYPE_WU_FLAG },
	{ "sedentaryremind", INV_SENSOR_TYPE_SEDENTARY_REMIND },
	{ "wsedentaryremind",INV_SENSOR_TYPE_SEDENTARY_REMIND | INV_SENSOR_TYPE_WU_FLAG },
	{ "dataencryption", INV_SENSOR_TYPE_DATA_ENCRYPTION },
	{ "wdataencryption",INV_SENSOR_TYPE_DATA_ENCRYPTION | INV_SENSOR_TYPE_WU_FLAG },
	{ "wom",       INV_SENSOR_TYPE_WOM },
	{ "wwom",      INV_SENSOR_TYPE_WOM | INV_SENSOR_TYPE_WU_FLAG },
	{ "hrgyr",    INV_SENSOR_TYPE_HIGH_RATE_GYRO },
	{ "whrgyr",    INV_SENSOR_TYPE_HIGH_RATE_GYRO | INV_SENSOR_TYPE_WU_FLAG },

	{ "cust0",     INV_SENSOR_TYPE_CUSTOM0 },
	{ "wcust0",    INV_SENSOR_TYPE_CUSTOM0 | INV_SENSOR_TYPE_WU_FLAG },
	{ "cust1",     INV_SENSOR_TYPE_CUSTOM1 },
	{ "wcust1",    INV_SENSOR_TYPE_CUSTOM1 | INV_SENSOR_TYPE_WU_FLAG },
	{ "cust2",     INV_SENSOR_TYPE_CUSTOM2 },
	{ "wcust2",    INV_SENSOR_TYPE_CUSTOM2 | INV_SENSOR_TYPE_WU_FLAG },
	{ "cust3",     INV_SENSOR_TYPE_CUSTOM3 },
	{ "wcust3",    INV_SENSOR_TYPE_CUSTOM3 | INV_SENSOR_TYPE_WU_FLAG },
	{ "cust4",     INV_SENSOR_TYPE_CUSTOM4 },
	{ "wcust4",    INV_SENSOR_TYPE_CUSTOM4 | INV_SENSOR_TYPE_WU_FLAG },
	{ "cust5",     INV_SENSOR_TYPE_CUSTOM5 },
	{ "wcust5",    INV_SENSOR_TYPE_CUSTOM5 | INV_SENSOR_TYPE_WU_FLAG },
	{ "cust6",     INV_SENSOR_TYPE_CUSTOM6 },
	{ "wcust6",    INV_SENSOR_TYPE_CUSTOM6 | INV_SENSOR_TYPE_WU_FLAG },
	{ "cust7",     INV_SENSOR_TYPE_CUSTOM7 },
	{ "wcust7",    INV_SENSOR_TYPE_CUSTOM7 | INV_SENSOR_TYPE_WU_FLAG }
	
};

int CommandSensor::findSensorFromShortName(const std::string & str)
{
	for(unsigned i = 0; i < sizeof(sensor_sname_map)/sizeof(sensor_sname_map[0]); ++i) {
		if(str.compare(sensor_sname_map[i].sname) == 0)
			return sensor_sname_map[i].id;
	}

	return -1;
}


int CommandSensor::getSensorIdFromString(const std::string & str)
{
	int id = findSensorFromShortName(str);

	if(id == -1) {
		if(!Poco::NumberParser::tryParse(str, id)) {
			unsigned tmp;
			if(!Poco::NumberParser::tryParseHex(str, tmp))
				return -1;
			
			id = static_cast<int>(tmp);
		}
				
		if(!INV_SENSOR_IS_VALID(id))
			return -1;
	}

	return id;
}

/******************************************************************************/

bool CommandSensorId::execute(const std::string & arg)
{
	(void)arg;

	for(unsigned i = 2 /* skip special all/wall sensors */;
			i < sizeof(sensor_sname_map)/sizeof(sensor_sname_map[0]); i += 2) {
		printf(" %2d [0x%02x] %s - %s\n",
				sensor_sname_map[i].id, sensor_sname_map[i].id,
				sensor_sname_map[i].sname,
				inv_sensor_2str(sensor_sname_map[i].id)
		);
	}
	
	printf("For wake-up variant, add 'w' prefix to short name.\n"
		   "eg: 'wacc' for wake-up acclerometer.\n");
	fflush(stdout);

	return true;
}

/******************************************************************************/

bool CommandSensorPing::execute(const std::string & arg)
{
	Poco::StringTokenizer tokens(arg, " \t",
			Poco::StringTokenizer::TOK_IGNORE_EMPTY |
			Poco::StringTokenizer:: TOK_TRIM);

	const unsigned count = tokens.count();
	int sensor = -1;

	if(count >= 1) {
		sensor = CommandSensor::getSensorIdFromString(tokens[0]);
	}

	if(INV_SENSOR_IS_VALID(sensor)) {
		bool ping = DeviceInstance::get().pingSensor(sensor);
		printf("Ping %s - %s (id: %d)\n", (ping) ? "OK" : "KO", inv_sensor_2str(sensor), sensor);
	} else {
		bool display_ko = (tokens.count() >= 1 && tokens[0] == "all") ? true : false;
		bool all_ko = true;

		for(unsigned int i = 1 /* skip reserved id */; i < INV_SENSOR_TYPE_MAX; ++i) {
			bool ping = DeviceInstance::get().pingSensor(i);

			if(ping) {
				printf("Ping OK - %s (id: %d)\n", inv_sensor_2str(i), i);
				all_ko = false;
			}
			else if(display_ko) {
				printf("Ping KO - %s (id: %d)\n", inv_sensor_2str(i), i);
			}
		}

		if(all_ko) {
			printf("No sensor available.\n");
		}
	}

	fflush(stdout);

	return true;
}

/******************************************************************************/

bool CommandSensorEnable::execute(const std::string & arg)
{
	int sensor, timeout;
	long period;

	parse(arg, sensor, period, timeout);

	if(INV_SENSOR_ID_TO_TYPE(sensor) == INV_SENSOR_TYPE_MAX) {
		for(int i = 1; i < INV_SENSOR_TYPE_MAX; ++i) {
			const int ss = i | (INV_SENSOR_IS_WU(sensor)*INV_SENSOR_TYPE_WU_FLAG);

			if(DeviceInstance::get().pingSensor(ss)) {
				if(period != -1)
					DeviceInstance::get().setSensorPeriodUs(ss, period);

				if(timeout != -1)
					DeviceInstance::get().setSensorTimeout(ss, timeout);

				DeviceInstance::get().startSensor(ss);
			}
		}

		return true;
	} else if(sensor != -1) {
		if(period != -1)
			DeviceInstance::get().setSensorPeriodUs(sensor, period);

		if(timeout != -1)
			DeviceInstance::get().setSensorTimeout(sensor, timeout);

		DeviceInstance::get().startSensor(sensor);

		return true;
	}

	return false;
}

void CommandSensorEnable::parse(const std::string & arg, int & sensor, long & period,
		int & timeout)
{
	Poco::StringTokenizer tokens(arg, " \t",
			Poco::StringTokenizer::TOK_IGNORE_EMPTY |
			Poco::StringTokenizer:: TOK_TRIM);

	const unsigned count = tokens.count();
	double fperiod;

	sensor  = -1;
	period  = -1;
	timeout = -1;

	if(count >= 1)
		sensor = CommandSensor::getSensorIdFromString(tokens[0]);

	if(count >= 2) {
		if(tokens[1].compare(0, 2, "1/") == 0) {
			if(Poco::NumberParser::tryParseFloat(tokens[1].substr(2), fperiod)) {
				period = static_cast<long>(1e6 / fperiod);
			}
		} else {
			if(Poco::NumberParser::tryParseFloat(tokens[1], fperiod))
				period = static_cast<long>(1e3 * fperiod);
		}
	}

	if(count < 3 || !Poco::NumberParser::tryParse(tokens[2], timeout))
		timeout = -1;
}

/******************************************************************************/

bool CommandSensorDisable::execute(const std::string & arg)
{
	Poco::StringTokenizer tokens(arg, " \t",
			Poco::StringTokenizer::TOK_IGNORE_EMPTY |
			Poco::StringTokenizer:: TOK_TRIM);

	const unsigned count = tokens.count();

	if(count >= 1) {

		const int sensor = CommandSensor::getSensorIdFromString(tokens[0]);

		if(INV_SENSOR_ID_TO_TYPE(sensor) == INV_SENSOR_TYPE_MAX) {
			for(int i = 1; i < INV_SENSOR_TYPE_MAX; ++i) {
				const int ss = i | (INV_SENSOR_IS_WU(sensor)*INV_SENSOR_TYPE_WU_FLAG);

				if(DeviceInstance::get().pingSensor(ss)) {
					DeviceInstance::get().stopSensor(ss);
				}
			}

			return true;
		} else if(sensor != -1) {
			DeviceInstance::get().stopSensor(sensor);
			return true;
		}
	}

	return false;
}

/******************************************************************************/

bool CommandSensorOdr::execute(const std::string & arg)
{
	int sensor;
	long period;

	parse(arg, sensor, period);

	if(period != -1) {
		if(INV_SENSOR_ID_TO_TYPE(sensor) == INV_SENSOR_TYPE_MAX) {
			for(int i = 1; i < INV_SENSOR_TYPE_MAX; ++i) {
				const int ss = i | (INV_SENSOR_IS_WU(sensor)*INV_SENSOR_TYPE_WU_FLAG);

				if(DeviceInstance::get().pingSensor(ss)) {
					DeviceInstance::get().setSensorPeriodUs(ss, period);
				}
			}

			return true;
		} else if(sensor != -1) {
			DeviceInstance::get().setSensorPeriodUs(sensor, period);
			return true;
		}
	}

	return false;
}

void CommandSensorOdr::parse(const std::string & arg, int & sensor, long & period)
{
	Poco::StringTokenizer tokens(arg, " \t",
			Poco::StringTokenizer::TOK_IGNORE_EMPTY |
			Poco::StringTokenizer:: TOK_TRIM);

	const unsigned count = tokens.count();
	double fperiod;

	sensor = -1;
	period = -1;

	if(count >= 2) {
		sensor = CommandSensor::getSensorIdFromString(tokens[0]);

		if(tokens[1].compare(0, 2, "1/") == 0) {
			if(Poco::NumberParser::tryParseFloat(tokens[1].substr(2), fperiod)) {
				period = static_cast<long>(1e6 / fperiod);
			}
		} else {
			if(Poco::NumberParser::tryParseFloat(tokens[1], fperiod))
				period = static_cast<long>(1e3 * fperiod);
		}
	}
}

/******************************************************************************/

bool CommandSensorBatch::execute(const std::string & arg)
{
	int sensor, timeout;

	parse(arg, sensor, timeout);

	if(timeout != -1) {
		if(INV_SENSOR_ID_TO_TYPE(sensor) == INV_SENSOR_TYPE_MAX) {
			for(int i = 1; i < INV_SENSOR_TYPE_MAX; ++i) {
				const int ss = i | (INV_SENSOR_IS_WU(sensor)*INV_SENSOR_TYPE_WU_FLAG);

				if(DeviceInstance::get().pingSensor(ss)) {
					DeviceInstance::get().setSensorTimeout(ss, timeout);
				}
			}
			return true;
		} else if(sensor != -1) {
			DeviceInstance::get().setSensorTimeout(sensor, timeout);
			return true;
		}
	}

	return false;
}

void CommandSensorBatch::parse(const std::string & arg, int & sensor, int & timeout)
{
	Poco::StringTokenizer tokens(arg, " \t",
			Poco::StringTokenizer::TOK_IGNORE_EMPTY |
			Poco::StringTokenizer:: TOK_TRIM);

	const unsigned count = tokens.count();

	if(count < 2) {
		sensor = -1;
		timeout = -1;
	} else {
		sensor = CommandSensor::getSensorIdFromString(tokens[0]);

		if(!Poco::NumberParser::tryParse(tokens[1], timeout))
			timeout = -1;
	}
}

/******************************************************************************/

bool CommandSensorFlush::execute(const std::string & arg)
{
	Poco::StringTokenizer tokens(arg, " \t",
			Poco::StringTokenizer::TOK_IGNORE_EMPTY |
			Poco::StringTokenizer:: TOK_TRIM);

	const unsigned count = tokens.count();

	if(count >= 1) {
		const int sensor = CommandSensor::getSensorIdFromString(tokens[0]);

		if(INV_SENSOR_ID_TO_TYPE(sensor) == INV_SENSOR_TYPE_MAX) {
			for(int i = 1; i < INV_SENSOR_TYPE_MAX; ++i) {
				const int ss = i | (INV_SENSOR_IS_WU(sensor)*INV_SENSOR_TYPE_WU_FLAG);

				if(DeviceInstance::get().pingSensor(ss)) {
					DeviceInstance::get().flushSensor(ss);
				}
			}
			return true;
		} else if(sensor != -1) {
			DeviceInstance::get().flushSensor(sensor);
			return true;
		}
	}

	return false;
}

/******************************************************************************/

bool CommandSensorGetData::execute(const std::string & arg)
{
	Poco::StringTokenizer tokens(arg, " \t",
			Poco::StringTokenizer::TOK_IGNORE_EMPTY |
			Poco::StringTokenizer:: TOK_TRIM);

	const unsigned count = tokens.count();

	if(count >= 1) {
		int sensor = CommandSensor::getSensorIdFromString(tokens[0]);

		if(sensor != -1) {
			inv_sensor_event_t event;
			DeviceInstance::get().getSensorData(sensor, event);

			printf("GET DATA %s (%d) t: %10llu data: ", inv_sensor_2str(event.sensor),
					event.sensor, event.timestamp);
			SensorEventsPrinter::instance().print(event, stdout);
			printf("\n");
			fflush(stdout);

			return true;
		}
	}

	return false;
}

/******************************************************************************/

bool CommandSensorPower::execute(const std::string & arg)
{
	Poco::StringTokenizer tokens(arg, " \t",
			Poco::StringTokenizer::TOK_IGNORE_EMPTY |
			Poco::StringTokenizer:: TOK_TRIM);

	const unsigned count = tokens.count();

	if(count >= 1) {
		if(tokens[0] == "run" || tokens[0] == "on") {
			DeviceInstance::get().setRunningState(true);
			return true;
		} else if(tokens[0] == "suspend" || tokens[0] == "off") {
			DeviceInstance::get().setRunningState(false);
			return true;
		}
	}

	return false;
}

/******************************************************************************/

bool CommandSensorSetBias::execute(const std::string & arg)
{
	Poco::StringTokenizer tokens(arg, " \t",
			Poco::StringTokenizer::TOK_IGNORE_EMPTY |
			Poco::StringTokenizer:: TOK_TRIM);

	const unsigned count = tokens.count();

	if(count >= 1) {
		int sensor = CommandSensor::getSensorIdFromString(tokens[0]);

		if(sensor != -1) {

			float bias[3] = { 0.f, 0.f, 0.f };

			if(tokens.count() >= 4) {
				for(unsigned i = 0; i < 3; ++i) {
					double d;

					if(Poco::NumberParser::tryParseFloat(tokens[1+i], d))
						bias[i] = static_cast<float>(d);
					else
						return false;
				}
			}

			DeviceInstance::get().setSensorBias(sensor, bias);

			return true;
		}
	}

	return false;
}

/******************************************************************************/

bool CommandSensorGetBias::execute(const std::string & arg)
{
	Poco::StringTokenizer tokens(arg, " \t",
			Poco::StringTokenizer::TOK_IGNORE_EMPTY |
			Poco::StringTokenizer:: TOK_TRIM);

	const unsigned count = tokens.count();

	if(count >= 1) {
		int sensor = CommandSensor::getSensorIdFromString(tokens[0]);

		if(sensor != -1) {
			float bias[3] = { 0.f, 0.f, 0.f };

			DeviceInstance::get().getSensorBias(sensor, bias);
			printf("GET BIAS %s (%d) bias: %f %f %f\n",
					inv_sensor_2str(sensor), sensor, bias[0], bias[1], bias[2]);
			fflush(stdout);

			return true;
		}
	}

	return false;
}

/******************************************************************************/

bool CommandSensorSetMatrix::execute(const std::string & arg)
{
	Poco::StringTokenizer tokens(arg, " \t",
			Poco::StringTokenizer::TOK_IGNORE_EMPTY |
			Poco::StringTokenizer:: TOK_TRIM);

	const unsigned count = tokens.count();

	if(count >= 1) {
		int sensor = CommandSensor::getSensorIdFromString(tokens[0]);

		if(sensor != -1) {

			float matrix[9] = { 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f };

			if(tokens.count() >= 10) {
				for(unsigned i = 0; i < 9; ++i) {
					double d;

					if(Poco::NumberParser::tryParseFloat(tokens[1+i], d))
						matrix[i] = static_cast<float>(d);
					else
						return false;
				}
			}

			DeviceInstance::get().setSensorMountingMatrix(sensor, matrix);

			return true;
		}
	}

	return false;
}

/******************************************************************************/

bool CommandSensorGetVersion::execute(const std::string & arg)
{
	(void)arg;

	inv_fw_version_t fw_info;

	DeviceInstance::get().getFwInfo(fw_info);
	printf("FW VERSION: %d.%d.%d%s CRC: 0x%x\n", fw_info.major, fw_info.minor,
			fw_info.patch, fw_info.suffix, fw_info.crc);
	fflush(stdout);

	return true;
}


/******************************************************************************/

bool CommandSensorSelfTest::execute(const std::string & arg)
{
	Poco::StringTokenizer tokens(arg, " \t",
			Poco::StringTokenizer::TOK_IGNORE_EMPTY |
			Poco::StringTokenizer:: TOK_TRIM);

	const unsigned count = tokens.count();

	if(count >= 1) {
		int sensor = CommandSensor::getSensorIdFromString(tokens[0]);

		if(sensor != -1) {
			const bool result = DeviceInstance::get().selfTest(sensor);

			printf("SELF TEST %s (%d) %s\n", inv_sensor_2str(sensor), sensor,
					(result) ? "OK" : "KO");
			fflush(stdout);

			return true;
		}
	}

	return false;
}

/******************************************************************************/

bool CommandSensorPollingPeriod::execute(const std::string & arg)
{
	Poco::StringTokenizer tokens(arg, " \t",
			Poco::StringTokenizer::TOK_IGNORE_EMPTY |
			Poco::StringTokenizer:: TOK_TRIM);

	const unsigned count = tokens.count();

	if(count >= 1) {
		int period;
		if(Poco::NumberParser::tryParse(tokens[0], period)) {
			if (period >= 0) {
				_poller.setPeriod(period);
				_poller.start();
			} else {
				_poller.stop();
			}
			return true;
		}
	}

	return false;
}

/******************************************************************************/

bool CommandSensorSetConfig::execute(const std::string & arg)
{
	Poco::StringTokenizer tokens(arg, " \t",
			Poco::StringTokenizer::TOK_IGNORE_EMPTY |
			Poco::StringTokenizer:: TOK_TRIM);

	const unsigned count = tokens.count();

	if(count < 2)
		return false;

	const int sensor = CommandSensor::getSensorIdFromString(tokens[0]);

	if(sensor == -1)
		return false;

	// retrieve setting string
	const size_t pos = tokens[1].find('=');

	if(pos != std::string::npos) {
		std::string setting = tokens[1].substr(0, pos);
		std::string payload = tokens[1].substr(pos+1);

		for(unsigned i = 2; i < count; ++i) {
			payload += " " + tokens[i];
		}

		// Setconfig for command : setconfig sensor settings=value
		return DeviceInstance::get().setSensorConfig(sensor, setting, payload);
	}
	// no setting name
	// parse string as hex
	else {
		std::string payload;

		for(unsigned i = 1; i < count; ++i) {
			unsigned data = 0;
			if(Poco::NumberParser::tryParseHex(tokens[i], data))
				payload.push_back((uint8_t)(data & 0xFF));
		}

		return DeviceInstance::get().setSensorConfig(sensor, "CUSTOM", payload);
	}
}

/******************************************************************************/

bool CommandSensorGetConfig::execute(const std::string & arg)
{
	Poco::StringTokenizer tokens(arg, " \t",
			Poco::StringTokenizer::TOK_IGNORE_EMPTY |
			Poco::StringTokenizer:: TOK_TRIM);

	const unsigned count = tokens.count();

	if(count < 1)
		return false;

	const int sensor = CommandSensor::getSensorIdFromString(tokens[0]);

	if(sensor == -1)
		return false;

	std::string value;

	if ( count == 2 )
	{
		const std::string& settings = tokens[1];

		if(DeviceInstance::get().getSensorConfig(sensor, settings, value)) 
		{
			printf("GET_CONFIG(%s) : %s=\n%s\n", inv_sensor_2str(sensor),
					settings.c_str(), value.c_str());
			fflush(stdout);
			return true;
		}
	}
	else
	{
		if(DeviceInstance::get().getSensorConfig(sensor, "CUSTOM", value)) 
		{
			printf("GET_CONFIG(%s) : CUSTOM=\n", inv_sensor_2str(sensor));

			for(unsigned i = 0; i < value.size(); i++) 
			{
				printf(" 0x%x", (uint8_t)value[i]);
			}
			printf("\n");
			fflush(stdout);
			return true;
		}
	}
	
	return false;
}

/******************************************************************************/

bool CommandWriteMemsReg::execute(const std::string & arg)
{
	Poco::StringTokenizer tokens(arg, " \t",
			Poco::StringTokenizer::TOK_IGNORE_EMPTY |
			Poco::StringTokenizer:: TOK_TRIM);

	const unsigned count = tokens.count();

	if(count < 3)
		return false;

	const int sensor = CommandSensor::getSensorIdFromString(tokens[0]);

	if(sensor == -1)
		return false;

	unsigned reg_addr = 0;
	std::string payload;

	if(!Poco::NumberParser::tryParseHex(tokens[1], reg_addr))
		return false;

	for(unsigned i = 2; i < count; i++) {
		unsigned value = 0;
		if(Poco::NumberParser::tryParseHex(tokens[i], value))
			payload.push_back((uint8_t)(value & 0xFF));
	}

	DeviceInstance::get().writeMemsRegister(sensor, (uint16_t)reg_addr,
			(const uint8_t *)&payload[0], payload.size());

	return true;
}

/******************************************************************************/

bool CommandReadMemsReg::execute(const std::string & arg)
{
	Poco::StringTokenizer tokens(arg, " \t",
			Poco::StringTokenizer::TOK_IGNORE_EMPTY |
			Poco::StringTokenizer:: TOK_TRIM);

	const unsigned count = tokens.count();

	if(count < 2)
		return false;

	const int sensor = CommandSensor::getSensorIdFromString(tokens[0]);

	if(sensor == -1)
		return false;

	unsigned reg_addr = 0;
	unsigned len = 1;
	uint8_t data[256];

	if(!Poco::NumberParser::tryParseHex(tokens[1], reg_addr))
		return false;

	if(count == 3) {
		if(!Poco::NumberParser::tryParseUnsigned(tokens[2], len))
			return false;

		if(len > sizeof(data))
			len = sizeof(data);
	}

	DeviceInstance::get().readMemsRegister(sensor, (uint16_t)reg_addr, data, len);

	printf("READ MEMS REG(%s) 0x%x[%d]=", inv_sensor_2str(sensor), reg_addr, len);

	for(unsigned i = 0; i < len; i++) {
		printf(" 0x%x", data[i]);
	}

	printf("\n");
	fflush(stdout);

	return true;
}

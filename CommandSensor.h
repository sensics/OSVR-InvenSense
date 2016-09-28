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

#ifndef _COMMAND_SENSOR_H_
#define _COMMAND_SENSOR_H_

#include "Command.h"

#include "Invn/Devices/SensorTypes.h"

#include <cstdio>

class DataEventPoller;

class CommandSensor
{
public:
	static int findSensorFromShortName(const std::string & str);
	static int getSensorIdFromString(const std::string & str);
};

/******************************************************************************/

class CommandSensorId : public Command
{
public:
	CommandSensorId()
		: Command("ids", "display sensor ids",
			"Display ids and short name for sensors to use in commands")
	{}

	bool execute(const std::string & arg);
};

/******************************************************************************/

class CommandSensorPing : public Command
{
public:
	CommandSensorPing()
		: Command("ping", "ping sensor to check if it supported by the device",
			"With no argument, will ping all sensors and list only the one reporting OK.\n"
			"With 'all' argument, will ping all sensors and list all sensors with OK/KO.")
	{}

	bool execute(const std::string & arg);
};

/******************************************************************************/

class CommandSensorEnable : public Command
{
public:
	CommandSensorEnable()
		: Command("en", "start a sensor",
			"Start a sensor designated by its short name or its id.\n"
			"If 'period' and 'timeout' are given, this command will also "
			"configure the sensor period and sensor batch timeout.\n"
			"Synopsys: enable sensor_id [period_ms] [timeout_ms]\n"
			"          enable sensor_id [1/odr_hz] [timeout_ms]\n"
			"          enable sensor_name [period_ms] [timeout_ms]\n"
			"          enable sensor_name [1/odr_hz] [timeout_ms]")
	{}

	bool execute(const std::string & arg);
private:
	void parse(const std::string & arg, int & sensor, long & period,
			int & timeout);
};

/******************************************************************************/

class CommandSensorDisable : public Command
{
public:
	CommandSensorDisable()
		: Command("dis", "stop a sensor",
			"Stop a sensor designated by its short name or its id.\n"
			"Synopsys: disable sensor_id")
	{}

	bool execute(const std::string & arg);
};

/******************************************************************************/

class CommandSensorOdr : public Command
{
public:
	CommandSensorOdr()
		: Command("odr", "change sensor output data rate",
			"Configure the requested period for reporting event for a sensor\n"
			"Synopsys: odr sensor_id period_ms\n"
			"          odr sensor_id 1/odr_hz")
	{}

	bool execute(const std::string & arg);
private:
	void parse(const std::string & arg, int & sensor, long & period);
};

/******************************************************************************/

class CommandSensorBatch : public Command
{
public:
	CommandSensorBatch()
		: Command("batch", "change sensor batch timeout",
			"Configure the requested batch timeout for a sensor\n"
			"Synopsys: batch sensor_id timeout_ms")
	{}

	bool execute(const std::string & arg);
private:
	void parse(const std::string & arg, int & sensor, int & timeout);
};

/******************************************************************************/

class CommandSensorFlush : public Command
{
public:
	CommandSensorFlush()
		: Command("flush", "flush sensor FIFO",
			"Request to flush all events for a sensor\n"
			"Synopsys: flush sensor_id")
	{}

	bool execute(const std::string & arg);
};

/******************************************************************************/

class CommandSensorGetData : public Command
{
public:
	CommandSensorGetData()
		: Command("getdata", "poll for last sensor data manually",
			"Retieve last acquired sensor data for a sensor\n"
			"Synopsys: getdata sensor_id")
	{}

	bool execute(const std::string & arg);
};

/******************************************************************************/

class CommandSensorPower : public Command
{
public:
	CommandSensorPower()
		: Command("power", "set device power (or running) state",
			"Synopsys: power run|on\n"
			"          power suspend|off"
		)
	{}

	bool execute(const std::string & arg);
};

/******************************************************************************/

class CommandSensorSetBias : public Command
{
public:
	CommandSensorSetBias()
		: Command("setbias", "set bias for sensor",
			"Synopsys: setbias sensor_id [bx by bz]\n"
			"\n"
			"If bx, by and bz are not specified, [0, 0, 0] is set."
		)
	{}

	bool execute(const std::string & arg);
};

/******************************************************************************/

class CommandSensorGetBias : public Command
{
public:
	CommandSensorGetBias()
		: Command("getbias", "get bias for sensor",
			"Synopsys: getbias sensor_id"
		)
	{}

	bool execute(const std::string & arg);
};

/******************************************************************************/

class CommandSensorSetMatrix : public Command
{
public:
	CommandSensorSetMatrix()
		: Command("setmatrix", "set mounting matrix for sensor",
			"Synopsys: setmatrix sensor_id [a b c d e f g h i]\n"
			"\n"
			"If matrix is not specified, identity matrix is set."
		)
	{}

	bool execute(const std::string & arg);
};

/******************************************************************************/

class CommandSensorGetVersion : public Command
{
public:
	CommandSensorGetVersion()
		: Command("getversion", "retrieve device firmware version",
			""
		)
	{}

	bool execute(const std::string & arg);
 };

/******************************************************************************/

class CommandSensorSelfTest : public Command
{
public:
	CommandSensorSelfTest()
		: Command("selftest", "perform MEMS self test for device",
			"Synopsys: selftest sensor_id"
		)
	{}

	bool execute(const std::string & arg);
};

/******************************************************************************/

class CommandSensorPollingPeriod : public Command
{
public:
	CommandSensorPollingPeriod(DataEventPoller& poller)
		: Command("pollingperiod", "configure period at which the device will be polled for events",
			"Synopsys: pollingperiod period_ms\n"
			"\n"
			"If period is -1, the device will not be polled anymore\n"
			"If period is 0,  the device will be polled without sleep"
		)
		, _poller(poller)
	{}

	bool execute(const std::string & arg);

private:
	DataEventPoller& _poller;
};

/******************************************************************************/

class CommandSensorSetConfig : public Command
{
public:
	CommandSensorSetConfig()
		: Command("setconfig", "configure sensor",
			"Synopsys: setconfig sensor settings1=value1 [settings2=value2] [...]\n"
			"        : setconfig sensor value0 [value1] [...]\n"
			"\n"
			"Refer to device documentation for available settings."
		)
	{}

	bool execute(const std::string & arg);
};

/******************************************************************************/

class CommandSensorGetConfig : public Command
{
public:
	CommandSensorGetConfig()
		: Command("getconfig", " retrieve sensor configuration",
			"Synopsys: getconfig sensor settings\n"
			"        : getconfig sensor\n"
			"\n"
			"Refer to device documentation for available settings."
		)
	{}

	bool execute(const std::string & arg);
};

/******************************************************************************/

class CommandWriteMemsReg : public Command
{
public:
	CommandWriteMemsReg()
		: Command("writememsreg", "Write to mems register",
			"Synopsys: writememsreg sensor_id register value0 [value1] [...]\n"
			"\n"
		)
	{}

	bool execute(const std::string & arg);
};


/******************************************************************************/

class CommandReadMemsReg : public Command
{
public:
	CommandReadMemsReg()
		: Command("readmemsreg", "Read from mems register",
			"Synopsys: readmemsreg sensor_id register [len]\n"
			"\n"
		)
	{}

	bool execute(const std::string & arg);
};

/******************************************************************************/

#endif /* _COMMAND_SENSOR_H_ */

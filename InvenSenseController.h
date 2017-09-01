/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_InvenSenseController_h_GUID_A17C0750_8743_4F88_A39A_379043D485BC
#define INCLUDED_InvenSenseController_h_GUID_A17C0750_8743_4F88_A39A_379043D485BC

#include "Invn/Devices/Device.h"
#include "Invn/Devices/Client/DeviceClient.h"
#include "Invn/EmbUtils/Message.h"
#include "Poco/Util/Application.h"
#include <osvr/Util/ReturnCodesC.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>

#include "DeviceInstance.h"
#include "Invn/Devices/Client/AsyncSensorEventsListener.h"
#include "Invn/Devices/Client/HostAdapterClient.h"
#include "Invn/Devices/Client/DataEventPoller.h"
#include "Invn/Devices/Client/WatchdogPoller.h"
#include "Invn/Devices/Client/SensorEventsDispatcher.h"

#include <memory>
#include <iostream>
#include <vector>
#include <string>

class InvenSenseController : public DeviceDebuggerHook,
                             public DeviceErrorHandler {
  public:
    InvenSenseController(const std::string &target, const std::string &port,
                         const std::string &adapter);
    ~InvenSenseController();

    /*
    @brief Enables sensor specified by sensor ID
    */
    OSVR_ReturnCode enableSensor(int sensorID);

    /*
    @brief Enables sensor specified by sensor ID and given ODR in microseconds
    */
    OSVR_ReturnCode enableSensor(int sensorID, unsigned long period_us);

    /*
    @brief Set sensor config by sensor ID and given settings and its respective value.
    */
    OSVR_ReturnCode setSensorConfig(int sensor, const std::string & settings, const std::string & value);

	/*
	@brief Get sesor config by sensor ID in given variable
	*/
	OSVR_ReturnCode InvenSenseController::getSensorConfig(int sensor, const std::string & settings, std::string & data);
	/*
    @brief Implements required function for DeviceErrorHandler
    */
    void handleDeviceError(DeviceClient *device, const std::exception &e);

    /*
    @brief Implements required function for DeviceDebuggerHook
    */
    void waitForDebugger(DeviceClient *device);

    static void eventCb(const inv_sensor_event_t *event, void *arg);

    SensorEventsDispatcher &getEventDispatcher();

    bool isDeviceConnected();

  private:
    /*
    @brief Establishes a connection to the device and sets up callbacks
    */
    OSVR_ReturnCode setupDevice();

    enum Target {
        TARGET_NONE,
        TARGET_DUMMY,
        TARGET_ICM3063X,
        TARGET_ICM30670,
        TARGET_VANADIUMEMD,
        TARGET_ICM20648,
        TARGET_ICM20948,
        TARGET_BERLIN,
        TARGET_ICM20602,
        TARGET_ICM20603,
        TARGET_ICM20690,
        TARGET_EMDWRAP_ICM20XXX,
        TARGET_EMDWRAP_ICM30XXX,
        TARGET_CHRE,
        TARGET_GSH,
    };

    enum TargetProxy {
        TARGET_PROXY_NONE,
        TARGET_PROXY_CMODEL,
    };

    bool _enable_target_debug;
    Target _selected_target;
    std::auto_ptr<HostAdapterClient> _serif_instance;
    std::auto_ptr<HostAdapterClient> _serif_instance_ois;
    std::auto_ptr<HostAdapterClient> _serif_instance_i2cslave;

    bool deviceConnected;

    /* device locker */
    DeviceLocker devLocker;

    /* create device */
    std::auto_ptr<DeviceClient> device;

    /* Asynchronous listener of data event */
    AsyncSensorEventsListener async_listener;

    /* poller for data events */
    DataEventPoller event_poller;
    /* poller for watchdog */
    WatchdogPoller watchdog_poller;

    /* pass events dispatcher (which is a listener) to device */
    SensorEventsDispatcher event_dispatcher;

    const std::string mAdapter;
    const std::string mPort;
    const std::string mTarget;
};

#endif // INCLUDED_InvenSenseController_h_GUID_A17C0750_8743_4F88_A39A_379043D485BC

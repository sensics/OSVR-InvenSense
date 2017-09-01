/** @file
    @brief Implementation

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

// Internal Includes
#include "InvenSenseController.h"
#include "Invn/Devices/Client/DeviceClientEmdWrapIcm20xxx.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include "stdbool.h"

static const auto PREFIX = "[InvenSense] ";

/*
InvenSenseController::InvenSenseController()
    : _serif_instance(0), _serif_instance_ois(0), _serif_instance_i2cslave(0),
      deviceLogger(""), event_poller(0), watchdog_poller(0),
      deviceConnected(false){};
      */
InvenSenseController::InvenSenseController(const std::string &target,
                                           const std::string &port,
                                           const std::string &adapter)
    :	_serif_instance(0), 
		_serif_instance_ois(0),
		_serif_instance_i2cslave(0),
		event_poller(0),
		watchdog_poller(0),
		deviceConnected(false),
		mTarget(target),
		mPort(port),
		mAdapter(adapter)
{

    OSVR_ReturnCode ret = setupDevice();

    if (ret == OSVR_RETURN_SUCCESS) {
        deviceConnected = true;
    }
}

InvenSenseController::~InvenSenseController() {

    if (deviceConnected) {
        try {
            /* clean-up device */
            event_poller.stop();
            watchdog_poller.stop();
            device->cleanup();
        } catch (const std::exception &e) {
            std::cerr << PREFIX << "Caught exception " << e.what()
                      << "on device clean-up" << std::endl;

        } catch (...) {
            std::cerr << PREFIX
                      << "Caught undefined exception on device clean-up"
                      << std::endl;
        }
    }
};

OSVR_ReturnCode InvenSenseController::setupDevice() {

    std::auto_ptr<HostAdapterClient> &ref_serif_instance = _serif_instance;

    ref_serif_instance.reset(HostAdapterClient::factoryCreate(mAdapter));
    if (!ref_serif_instance.get()) {
        std::cerr << "Adapter " << mAdapter << " unknown. Aborting"
                  << std::endl;
        return OSVR_RETURN_FAILURE;
    }

    INV_MSG_SETUP(INV_MSG_LEVEL_DEBUG, inv_msg_printer_default);

    assert(_serif_instance.get());

    try {
        _serif_instance->open();
    } catch (const std::exception &e) {
        std::cerr << PREFIX << "Caught exception" << e.what()
                  << "while openning serial interface" << std::endl;
        return OSVR_RETURN_FAILURE;
    } catch (...) {
        std::cerr
            << PREFIX
            << "Caught undefined exception while openning serial interface"
            << std::endl;
        return OSVR_RETURN_FAILURE;
    }

    if (_serif_instance_ois.get()) {
        try {
            _serif_instance_ois->open();
        } catch (const std::exception &e) {
            std::cerr << PREFIX << "Caught exception" << e.what()
                      << "while openning serial interface" << std::endl;
            return OSVR_RETURN_FAILURE;
        } catch (...) {
            std::cerr
                << PREFIX
                << "Caught undefined exception while openning serial interface"
                << std::endl;
            return OSVR_RETURN_FAILURE;
        }
    }
    if (_serif_instance_i2cslave.get()) {
        try {
            _serif_instance_i2cslave->open();
        } catch (const std::exception &e) {
            std::cerr << PREFIX << "Caught exception" << e.what()
                      << "while openning serial interface" << std::endl;
            return OSVR_RETURN_FAILURE;
        } catch (...) {
            std::cerr
                << PREFIX
                << "Caught undefined exception while openning serial interface"
                << std::endl;
            return OSVR_RETURN_FAILURE;
        }
    }

    _selected_target = TARGET_EMDWRAP_ICM20XXX;
    switch (_selected_target) {
    case TARGET_EMDWRAP_ICM20XXX:
        device.reset(new DeviceClientEmdWrapIcm20xxx(mPort));
        break;
    default:
        assert(0);
        return OSVR_RETURN_FAILURE;
    }

    /* set Device global instance */
    DeviceInstance::set(*device);

    event_poller.setDevice(device.get());
    watchdog_poller.setDevice(device.get());

    /* setup device */
    try {
        /* attach locker */
        device->setLocker(&devLocker);

        /* attach listeners */
        async_listener.setListener(&event_dispatcher);
        device->setListener(&async_listener);

        if (_enable_target_debug) {
            device->registerDebuggerHook(this);
        }
        device->setup();
        async_listener.start();
        event_poller.registerErrorHandler(this);
        event_poller.start();
        watchdog_poller.registerErrorHandler(this);
        watchdog_poller.start();
        deviceConnected = true;
    } catch (const std::exception &e) {
        std::cerr << PREFIX << "Encountered exception " << e.what()
                  << " during device setup" << std::endl;
        return OSVR_RETURN_FAILURE;
    } catch (...) {
        std::cerr << PREFIX
                  << "Encountered unknown exception during device setup "
                  << std::endl;
        return OSVR_RETURN_FAILURE;
    }

    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode InvenSenseController::enableSensor(int sensorID) {

    device->startSensor(sensorID);

    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode InvenSenseController::enableSensor(int sensorID,
                                                   unsigned long period_us) {

    device->setSensorPeriodUs(sensorID, period_us);
    device->startSensor(sensorID);

    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode InvenSenseController::setSensorConfig(int sensor, const std::string & settings, const std::string & value){
	device->setSensorConfig(sensor, settings, value);
	return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode InvenSenseController::getSensorConfig(int sensor, const std::string & settings, std::string & data){
	device->getSensorConfig(sensor, settings, data);
	return OSVR_RETURN_SUCCESS;
}



void InvenSenseController::waitForDebugger(DeviceClient *device) {
    std::cout << "waitForDebugger" << std::endl;
}

void InvenSenseController::handleDeviceError(DeviceClient *device,
                                             const std::exception &e) {
    (void)device;

    std::cerr << "Got error " << e.what() << " while polling device."
              << std::endl;
}

SensorEventsDispatcher &InvenSenseController::getEventDispatcher() {
    return event_dispatcher;
}

bool InvenSenseController::isDeviceConnected() { return deviceConnected; }
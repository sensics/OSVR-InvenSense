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

#include "Invn/Devices/Client/DeviceClientIcm30xxx.h"
#include "Invn/Devices/Client/DeviceClientVanadiumEMD.h"
#include "Invn/Devices/Client/DeviceClientIcm20648.h"
#include "Invn/Devices/Client/DeviceClientDummy.h"
#include "Invn/Devices/Client/DeviceClientSmartMotion.h"
#include "Invn/Devices/Client/DeviceClientIcm20602.h"
#include "Invn/Devices/Client/DeviceClientIcm20603.h"
#include "Invn/Devices/Client/DeviceClientIcm20690.h"
#include "Invn/Devices/Client/DeviceClientEmdWrapper.h"
#include "Invn/Devices/Client/DeviceClientEmdWrapIcm30xxx.h"
#include "Invn/Devices/Client/DeviceClientGsh.h"
#include "Invn/Devices/Client/DeviceClientChre.h"
#include "Invn/Devices/HostAdapter/CheetahAdapter.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include "stdbool.h"

static const auto PREFIX = "[InvenSense] ";

InvenSenseController::InvenSenseController()
    : _serif_instance(0), _serif_instance_ois(0), _serif_instance_i2cslave(0),
      deviceLogger(""), event_poller(0), watchdog_poller(0){};

InvenSenseController::~InvenSenseController() {

    try {
        /* clean-up device */
        event_poller.stop();
        watchdog_poller.stop();
        device->cleanup();
    } catch (const std::exception &e) {
        std::cerr << PREFIX << "Caught exception " << e.what()
                  << "on device clean-up" << std::endl;

    } catch (...) {
        std::cerr << PREFIX << "Caught undefined exception on device clean-up"
                  << std::endl;
    }
};

OSVR_ReturnCode InvenSenseController::connect(const std::string &target,
                                              const std::string &port) {

    const std::string &adapter = "dummy";
    const std::string &name = "adapter";

    std::auto_ptr<HostAdapterClient> &ref_serif_instance =
        (name == "adapter") ? _serif_instance : (name == "adapter2")
                                                    ? _serif_instance_ois
                                                    : _serif_instance_i2cslave;

    ref_serif_instance.reset(HostAdapterClient::factoryCreate(adapter));
    if (!ref_serif_instance.get()) {
        std::cerr << "Adapter " << adapter << " unknown. Aborting" << std::endl;
        return OSVR_RETURN_FAILURE;
    }

    INV_MSG_SETUP(INV_MSG_LEVEL_DEBUG, inv_msg_printer_default);

    assert(_serif_instance.get());

    try {
        _serif_instance->open();
    } catch (const std::exception &e) {
        std::cerr << PREFIX << PREFIX << "Caught exception" << e.what()
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

    uint8_t selectedTarget = TARGET_EMDWRAPPER;
    switch (selectedTarget) {
    case TARGET_EMDWRAPPER:
        device.reset(new DeviceClientEmdWrapper(port));
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

OSVR_ReturnCode InvenSenseController::enableTracking() {

    device->startSensor(INV_SENSOR_TYPE_GAME_ROTATION_VECTOR);

    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode InvenSenseController::getTracking(OSVR_OrientationState *data) {

    int sensor;
    sensor = INV_SENSOR_TYPE_GAME_ROTATION_VECTOR;
    inv_sensor_event_t event;
    device->getSensorData(sensor, event);
    std::cout << "w: " << event.data.quaternion.quat[0]
              << "x: " << event.data.quaternion.quat[1]
              << "y: " << event.data.quaternion.quat[2]
              << "z: " << event.data.quaternion.quat[3] << std::endl;

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
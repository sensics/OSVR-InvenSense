/** @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics Corporation.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/Util/PlatformConfig.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>
#include "com_sensics_InvenSense_json.h"
#include "InvenSenseController.h"

#include "Invn/Devices/Client/SensorEventsDispatcher.h"
#include "Invn/HostUtils/NamedPipe.h"

// Library/third-party includes
#include <math.h>
#include <json/reader.h>
#include <json/value.h>

// Standard includes
#include <deque>
#include <iostream>
#include <memory>
#include <vector>

// Anonymous namespace to avoid symbol collision
namespace {

static const auto DRIVER_NAME = "OSVR_InvenSense";
static const auto PREFIX = "[OSVR-InvenSense] ";
typedef std::shared_ptr<InvenSenseController> InvnCtlPtr;

inline const std::string getTargetDefault() { return "emdwrapper"; }
inline const std::string getAdapterDefault() { return "dummy"; }

class InvenSenseDevice : public SensorEventsListener {
  public:
    ~InvenSenseDevice() { _dispatcher.unsubscribe(this); }
    InvenSenseDevice(OSVR_PluginRegContext ctx, InvnCtlPtr controller)
        : m_controller(controller),
          _dispatcher(controller->getEventDispatcher())

    {
        osvrTimeValueGetNow(&m_last);
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        osvrDeviceTrackerConfigure(opts, &m_tracker);

        /// Create the sync device token with the options
        m_dev.initSync(ctx, "InvenSense", opts);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(com_sensics_InvenSense_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);
        _dispatcher.subscribe(this);

        controller->enableGRV();
    }

    OSVR_ReturnCode InvenSenseDevice::update() { return OSVR_RETURN_SUCCESS; }

    void InvenSenseDevice::notify(const inv_sensor_event_t &event) {

        OSVR_OrientationState orientation;
        orientation.data[0] = event.data.quaternion.quat[0];
        orientation.data[1] = event.data.quaternion.quat[1];
        orientation.data[2] = event.data.quaternion.quat[2];
        orientation.data[3] = event.data.quaternion.quat[3];
        OSVR_TimeValue timestamp;
        osvrTimeValueGetNow(&timestamp);
        osvrDeviceTrackerSendOrientationTimestamped(
            m_dev, m_tracker, &orientation, 0, &timestamp);
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_TrackerDeviceInterface m_tracker;
    InvnCtlPtr m_controller;
    OSVR_TimeValue m_last;
    SensorEventsDispatcher &_dispatcher;
};

class InvenSensePluginInstantiation {

  public:
    InvenSensePluginInstantiation() : m_found(false) {}

    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx, const char *params) {

        if (m_found) {
            return OSVR_RETURN_SUCCESS;
        }

        Json::Value root;
        {
            Json::Reader reader;
            if (!reader.parse(params, root)) {
                std::cerr << PREFIX
                          << "Could not parse JSON for OSVR InvenSense plugin"
                          << std::endl;
                return OSVR_RETURN_FAILURE;
            }
        }

        std::string portParam, targetParam, adapterParam;
        if (!root.isMember("port")) {
            std::cerr << PREFIX << "Could not find port parameter. Verify that "
                                   "port is specified in the config file"
                      << std::endl;
            return OSVR_RETURN_FAILURE;
        } else {
            portParam = root["port"].asString();
            if (portParam.empty()) {
                std::cerr << PREFIX
                          << "Could not find port value. Verify that port "
                             "value is specified in the config file"
                          << std::endl;
                return OSVR_RETURN_FAILURE;
            }
        }

        if (!root.isMember("target")) {
            targetParam = getTargetDefault();
            std::cout << PREFIX
                      << "Could not find target param. Using default \""
                      << targetParam << "\" parameter" << std::endl;
        } else {
            targetParam = root["target"].asString();
        }

        if (!root.isMember("adapter")) {
            adapterParam = getAdapterDefault();
            std::cout << PREFIX
                      << "Could not find adapter param. Using default \""
                      << targetParam << "\" parameter" << std::endl;
        } else {
            adapterParam = root["adapter"].asString();
        }

        controller = std::shared_ptr<InvenSenseController>(
            new InvenSenseController(targetParam, portParam, adapterParam));

        if (controller->isDeviceConnected()) {
            std::cout << PREFIX << "Detected InvenSense device! " << std::endl;
            m_found = true;

            /// Create our device object
            osvr::pluginkit::registerObjectForDeletion(
                ctx, new InvenSenseDevice(ctx, controller));
        } else {
            std::cout << PREFIX << "NOT detected InvenSense tracker "
                      << std::endl;
            return OSVR_RETURN_FAILURE;
        }

        return OSVR_RETURN_SUCCESS;
    }

  private:
    InvnCtlPtr controller;
    bool m_found;
};
} // namespace

OSVR_PLUGIN(com_sensics_InvenSense) {
    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerDriverInstantiationCallback(
        DRIVER_NAME, InvenSensePluginInstantiation());

    return OSVR_RETURN_SUCCESS;
}

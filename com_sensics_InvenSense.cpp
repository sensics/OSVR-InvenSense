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

// Library/third-party includes
#include <math.h>

// Standard includes
#include <deque>
#include <iostream>
#include <memory>
#include <vector>

// Anonymous namespace to avoid symbol collision
namespace {

static const auto PREFIX = "[OSVR-InvenSense] ";
typedef std::shared_ptr<InvenSenseController> InvnCtlPtr;

class InvenSenseDevice {
  public:
    InvenSenseDevice(OSVR_PluginRegContext ctx, InvnCtlPtr controller)
        : m_controller(controller)

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
    }

    OSVR_ReturnCode InvenSenseDevice::update() {

        OSVR_OrientationState orientation;
        m_controller->getTracking(&orientation);

        return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_TrackerDeviceInterface m_tracker;
    InvnCtlPtr m_controller;
    OSVR_TimeValue m_last;
};

class HardwareDetection {

  public:
    HardwareDetection()
        : controller(new InvenSenseController()), m_found(false) {}

    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {

        OSVR_ReturnCode ret = controller->connect("emdwrapper", "COM13");

        if (ret == OSVR_RETURN_SUCCESS) {
            std::cout << "PLUGIN: We have detected InvenSense device! "
                      << std::endl;
            m_found = true;
            ret = controller->enableTracking();
            /// Create our device object
            osvr::pluginkit::registerObjectForDeletion(
                ctx, new InvenSenseDevice(ctx, controller));
        } else {
            std::cout << PREFIX << "We have NOT detected InvenSense tracker "
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
    context.registerHardwareDetectCallback(new HardwareDetection());

    return OSVR_RETURN_SUCCESS;
}

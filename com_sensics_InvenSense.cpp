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

class HardwareDetection {

  public:
    HardwareDetection() {}

    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {

        InvenSenseController controller;
        controller.connect("emdwrapper", "COM13");

        return OSVR_RETURN_SUCCESS;
    }

  private:
    // InvnCtlPtr controller;
    bool mFound;
};
} // namespace

OSVR_PLUGIN(com_sensics_InvenSense) {
    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerHardwareDetectCallback(new HardwareDetection());

    return OSVR_RETURN_SUCCESS;
}

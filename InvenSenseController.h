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

#include <memory>
#include <iostream>
#include <vector>
#include <string>

class InvenSenseController {
  public:
    InvenSenseController();
    ~InvenSenseController();

    OSVR_ReturnCode connect(const std::string &target, const std::string &port);
    OSVR_ReturnCode enableTracking();
    OSVR_ReturnCode getTracking(OSVR_OrientationState *data);

  private:
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
        TARGET_EMDWRAPPER,
        TARGET_EMDWRAP_ICM30XXX,
        TARGET_CHRE,
        TARGET_GSH,
    };

    enum TargetProxy {
        TARGET_PROXY_NONE,
        TARGET_PROXY_CMODEL,
    };

    // ExitCode					_exit_code;
    bool _enable_target_debug;
    Target _selected_target;
    TargetProxy _selected_target_proxy;
    int _msg_level;
    bool _fw_image_force;
    bool _fw_image_verify;
    std::vector<std::string> _commands_from_argv;
    std::string _fw_image_path;
    std::string _fw_ram_image_path;
    std::string _dmp3_image_path;
    std::string _dmp4_image_path;
    std::string _zsp_imem_image_path;
    std::string _zsp_dmem_image_path;
    std::string _device_logger_file;
    std::string _device_logger_level;
    std::auto_ptr<HostAdapterClient> _serif_instance;
    std::auto_ptr<HostAdapterClient> _serif_instance_ois;
    std::auto_ptr<HostAdapterClient> _serif_instance_i2cslave;

    const std::string mPort;
    const std::string mTarget;

    /* device locker */
    DeviceLocker devLocker;

    /* create device */
    std::auto_ptr<DeviceClient> device;
};

#endif // INCLUDED_InvenSenseController_h_GUID_A17C0750_8743_4F88_A39A_379043D485BC

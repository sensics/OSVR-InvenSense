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
#include <quat/quat.h>

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

static const double invensense_DT = 1.0 / 1000;

class InvenSenseDevice : public SensorEventsListener {
  public:
    ~InvenSenseDevice() { _dispatcher.unsubscribe(this); }
    InvenSenseDevice(OSVR_PluginRegContext ctx, InvnCtlPtr controller, unsigned long prediction_time, bool prediction_en)
        : m_controller(controller),
          _dispatcher(controller->getEventDispatcher()), m_gyro_cal(false),
		  m_prediction_enabled(prediction_en),m_prediction_time(prediction_time)

    {
        osvrTimeValueGetNow(&m_last);
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        osvrDeviceTrackerConfigure(opts, &m_tracker);

        /// Create the sync device token with the options
        m_dev.initAsync(ctx, "InvenSense", opts);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(com_sensics_InvenSense_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);
        _dispatcher.subscribe(this);
        // Enable Gyroscope vector at 1KHz.
        controller->enableSensor(INV_SENSOR_TYPE_GYROSCOPE, 1000);
    }

    OSVR_ReturnCode InvenSenseDevice::update() { return OSVR_RETURN_SUCCESS; }
	
	void InvenSenseDevice::setPredictionTime(unsigned long time){
		m_prediction_time = time;
	}

	void InvenSenseDevice::setPredictionEnabled(bool enabled){
		m_prediction_enabled = enabled;
	}

    void InvenSenseDevice::notify(const inv_sensor_event_t &event) {
        int sensorId = (int)(event.sensor & ~INV_SENSOR_TYPE_WU_FLAG);
        OSVR_OrientationState orientation;
        OSVR_TimeValue timestamp;
        switch (sensorId) {
		case INV_SENSOR_TYPE_GAME_ROTATION_VECTOR:
        case INV_SENSOR_TYPE_PRED_QUAT_1:
            orientation.data[0] = event.data.quaternion.quat[0];
            orientation.data[1] = event.data.quaternion.quat[1];
            orientation.data[2] = event.data.quaternion.quat[2];
            orientation.data[3] = event.data.quaternion.quat[3];
            osvrTimeValueGetNow(&timestamp);
            osvrDeviceTrackerSendOrientationTimestamped(
                m_dev, m_tracker, &orientation, 0, &timestamp);
            break;

        case INV_SENSOR_TYPE_GYROSCOPE:

            // Wait for the calibration of gyro before starting game rotation
            // vector.
            if (!m_gyro_cal && event.data.gyr.accuracy_flag == 3) {
                m_gyro_cal = true;
                // Enable Game Rotation vector at 1KHz.
				if(m_prediction_enabled){
					std::string payload = "2 " + std::to_string((unsigned long long)m_prediction_time);
					m_controller->setSensorConfig(INV_SENSOR_TYPE_PRED_QUAT_1, "pred_quat", payload );
					m_controller->enableSensor(INV_SENSOR_TYPE_PRED_QUAT_1, 1000);
				}else{
					m_controller->enableSensor(INV_SENSOR_TYPE_GAME_ROTATION_VECTOR, 1000);
				}
            }
            // If gyro calibration is done use prediction
            if (m_gyro_cal) {
                q_type forward, inverse;
                q_type delta;
                q_type canonical;
                double vel_quat[4];
                double vel_quat_dt = 0.0;
                OSVR_VelocityState vel;
                q_copy(forward, inverse);
                q_invert(inverse, forward);
                delta[Q_W] = 0;
                delta[Q_X] =
                    event.data.gyr.vect[0] * invensense_DT * 0.5 * 0.0174533;
                delta[Q_Y] =
                    event.data.gyr.vect[1] * invensense_DT * 0.5 * 0.0174533;
                delta[Q_Z] =
                    event.data.gyr.vect[2] * invensense_DT * 0.5 * 0.0174533;

                q_exp(delta, delta);
                q_normalize(delta, delta);
                q_mult(canonical, delta, inverse);
                vel_quat_dt = invensense_DT;
                vel_quat[0] = vel_quat[1] = vel_quat[2] = 0.0;
                vel_quat[3] = 1.0;
                q_mult(vel_quat, forward, canonical);
                vel.angularVelocity.dt = vel_quat_dt;
                vel.angularVelocity.incrementalRotation.data[Q_W] =
                    vel_quat[Q_W];
                vel.angularVelocity.incrementalRotation.data[Q_X] =
                    vel_quat[Q_X];
                vel.angularVelocity.incrementalRotation.data[Q_Y] =
                    vel_quat[Q_Y];
                vel.angularVelocity.incrementalRotation.data[Q_Z] =
                    vel_quat[Q_Z];

                osvrTimeValueGetNow(&timestamp);
                osvrDeviceTrackerSendVelocityTimestamped(m_dev, m_tracker, &vel,
                                                         0, &timestamp);
            }
            break;

        default:
            break;
        }
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_TrackerDeviceInterface m_tracker;
    InvnCtlPtr m_controller;
    OSVR_TimeValue m_last;
    SensorEventsDispatcher &_dispatcher;
    bool m_gyro_cal;
	unsigned long m_prediction_time;
	bool m_prediction_enabled;
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
		unsigned long prediction_time;
		bool prediction_enabled;

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

        if (!root.isMember("prediction")) {
			prediction_enabled = true;
			prediction_time = 3000;
            std::cout << PREFIX
                      << "Could not find prediction param. Enabling prediction with 3ms " << std::endl;
        } else {
			Json::Value& prediction = root["prediction"];
			prediction_enabled = prediction["enabled"].asBool();
			prediction_time = prediction["time"].asUInt();
            std::cout << PREFIX
				<< "Setting up prediction from JSON" << std::endl;
			std::cout << PREFIX
                      << "prediction enabled: " << prediction_enabled <<std::endl;
			std::cout  << PREFIX 
				<< "prediction time: " << prediction_time << std::endl;

        }

		controller = std::shared_ptr<InvenSenseController>(
			new InvenSenseController(targetParam, portParam, adapterParam));

		if (controller->isDeviceConnected()) {
            std::cout << PREFIX << "Detected InvenSense device! " << std::endl;
            m_found = true;

            /// Create our device object
			InvenSenseDevice* pInvnDevice = new InvenSenseDevice(ctx, controller, prediction_time, prediction_enabled);
            osvr::pluginkit::registerObjectForDeletion(
                ctx, pInvnDevice);
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

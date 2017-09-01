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
#include <fstream>

// Anonymous namespace to avoid symbol collision
namespace {

	static const auto DRIVER_NAME = "OSVR_InvenSense";
	static const auto PREFIX = "[OSVR-InvenSense] ";
	typedef std::shared_ptr<InvenSenseController> InvnCtlPtr;

	inline const std::string getTargetDefault() { return "emdwrapper"; }
	inline const std::string getAdapterDefault() { return "dummy"; }

	double invensense_DT = 1.0 / 1000;
	std::ofstream data_stream;

	class InvenSenseDevice : public SensorEventsListener {
	public:
		~InvenSenseDevice() { _dispatcher.unsubscribe(this); }
		InvenSenseDevice(OSVR_PluginRegContext ctx, InvnCtlPtr controller, long prediction_time, bool prediction_en, unsigned long periode, std::string fsrGyr, std::string fsrAcc,
			bool datafile, bool logacc, bool loggyr, bool logpq1, bool loggrv, bool logracc, bool logrgyr, bool logrmag, bool logrtmp)
			: m_controller(controller),
			_dispatcher(controller->getEventDispatcher()), m_gyro_cal(false),
			m_prediction_enabled(prediction_en), m_prediction_time(prediction_time), m_periode(periode),
			m_fsrGyr(fsrGyr), m_fsrAcc(fsrAcc),
			m_datafile(datafile), m_logacc(logacc), m_loggyr(loggyr), m_logpq1(logpq1), m_loggrv(loggrv),
			m_logracc(logracc), m_logrgyr(logrgyr), m_logrmag(logrmag), m_logrtmp(logrtmp)

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

			data_stream << "# Config: Prediction enabled=" << m_prediction_enabled
				<< " Prediction time=" << m_prediction_time
				<< " Periode=" << m_periode
				<< " GyrFSR=" << m_fsrGyr
				<< " AccFSR=" << m_fsrAcc << std::endl;

			// Set Gyroscope and Accelerometer FSR
			//controller->setSensorConfig(INV_SENSOR_TYPE_GYROSCOPE, "fsr", m_fsrGyr);
			//controller->setSensorConfig(INV_SENSOR_TYPE_ACCELEROMETER, "fsr", m_fsrAcc);

			// Enable Gyroscope and Accelerometer at the chosen periode
			controller->enableSensor(INV_SENSOR_TYPE_GYROSCOPE, m_periode);
			controller->enableSensor(INV_SENSOR_TYPE_ACCELEROMETER, m_periode);

			// Enable sensors for logging if needed
			if (m_logracc)
				controller->enableSensor(INV_SENSOR_TYPE_RAW_ACCELEROMETER, m_periode);

			if (m_logrgyr)
				controller->enableSensor(INV_SENSOR_TYPE_RAW_GYROSCOPE, m_periode);

			if (m_logrmag)
				controller->enableSensor(INV_SENSOR_TYPE_RAW_MAGNETOMETER);

			if (m_logrtmp)
				controller->enableSensor(INV_SENSOR_TYPE_RAW_TEMPERATURE, m_periode);

			// Enable GRV in any case
			controller->enableSensor(INV_SENSOR_TYPE_GAME_ROTATION_VECTOR, m_periode);

			// If predictive quat peridode is negative, create a buffer to add latency
			if (m_prediction_time < 0) {
				m_latencyadding = true;
				m_buffer_size = (long)(1 - (long)m_periode * (long)m_prediction_time / 1000000);
				if (m_buffer_size > 1000 || m_buffer_size < 1){
					m_buffer_size = 1000;
				}
				m_prediction_time = 0; // set to 0 to start predictive quat
				m_buffer_pos = 0;
			} 
			else {
				m_latencyadding = false;
			}

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
			char sensorIdhexa[10];
			sprintf(sensorIdhexa, "0x%08x", sensorId);
			switch (sensorId) {

			case INV_SENSOR_TYPE_RAW_ACCELEROMETER:
				if (m_datafile && m_logracc){
					if (data_stream.is_open())
					{
						data_stream << "D "
							<< inv_sensor_2str(sensorId) << " "
							<< sensorIdhexa << " "
							<< event.status << " "
							<< event.timestamp << " "
							<< event.data.raw3d.vect[0] << " "
							<< event.data.raw3d.vect[1] << " "
							<< event.data.raw3d.vect[2] << std::endl;
					}
				}
				break;

			case INV_SENSOR_TYPE_RAW_GYROSCOPE:
				if (m_datafile && m_logrgyr){
					if (data_stream.is_open())
					{
						data_stream << "D "
							<< inv_sensor_2str(sensorId) << " "
							<< sensorIdhexa << " "
							<< event.status << " "
							<< event.timestamp << " "
							<< event.data.raw3d.vect[0] << " "
							<< event.data.raw3d.vect[1] << " "
							<< event.data.raw3d.vect[2] << std::endl;
					}
				}
				break;

			case INV_SENSOR_TYPE_RAW_MAGNETOMETER:
				if (m_datafile && m_logrmag){
					if (data_stream.is_open())
					{
						data_stream << "D "
							<< inv_sensor_2str(sensorId) << " "
							<< sensorIdhexa << " "
							<< event.status << " "
							<< event.timestamp << " "
							<< event.data.raw3d.vect[0] << " "
							<< event.data.raw3d.vect[1] << " "
							<< event.data.raw3d.vect[2] << std::endl;
					}
				}
				break;

			case INV_SENSOR_TYPE_RAW_TEMPERATURE:
				if (m_datafile && m_logrtmp){
					if (data_stream.is_open())
					{
						data_stream << "D "
							<< inv_sensor_2str(sensorId) << " "
							<< sensorIdhexa << " "
							<< event.status << " "
							<< event.timestamp << " "
							<< event.data.rawtemp.raw << std::endl;
					}
				}
				break;

			case INV_SENSOR_TYPE_GAME_ROTATION_VECTOR:

				orientation.data[0] = event.data.quaternion.quat[0];
				orientation.data[1] = event.data.quaternion.quat[1];
				orientation.data[2] = event.data.quaternion.quat[2];
				orientation.data[3] = event.data.quaternion.quat[3];
				osvrTimeValueGetNow(&timestamp);

				if (m_latencyadding)
				{
					m_quat_buffer[m_buffer_pos][0] = orientation.data[0];
					m_quat_buffer[m_buffer_pos][1] = orientation.data[1];
					m_quat_buffer[m_buffer_pos][2] = orientation.data[2];
					m_quat_buffer[m_buffer_pos][3] = orientation.data[3];

					orientation.data[0] = m_quat_buffer[(m_buffer_pos + 1) % m_buffer_size][0];
					orientation.data[1] = m_quat_buffer[(m_buffer_pos + 1) % m_buffer_size][1];
					orientation.data[2] = m_quat_buffer[(m_buffer_pos + 1) % m_buffer_size][2];
					orientation.data[3] = m_quat_buffer[(m_buffer_pos + 1) % m_buffer_size][3];

					m_buffer_pos = (m_buffer_pos + 1) % m_buffer_size;

					osvrDeviceTrackerSendOrientationTimestamped(m_dev, m_tracker, &orientation, 0, &timestamp);

				}

				if (m_datafile && m_loggrv){
					if (data_stream.is_open())
					{
						data_stream << "D "
							<< inv_sensor_2str(sensorId) << " "
							<< sensorIdhexa << " "
							<< event.status << " "
							<< event.timestamp << " "
							<< orientation.data[0] << " "
							<< orientation.data[1] << " "
							<< orientation.data[2] << " "
							<< orientation.data[3] << " "
							<< int(event.data.quaternion.accuracy_flag) << std::endl;
					}
				}
				break;

			case INV_SENSOR_TYPE_PRED_QUAT_1:
				orientation.data[0] =  event.data.quaternion.quat[0];   
				orientation.data[1] =  event.data.quaternion.quat[1];
				orientation.data[2] =  event.data.quaternion.quat[2];
				orientation.data[3] =  event.data.quaternion.quat[3];
				osvrTimeValueGetNow(&timestamp);
				osvrDeviceTrackerSendOrientationTimestamped(
					m_dev, m_tracker, &orientation, 0, &timestamp);

				if (m_datafile && m_logpq1){
					if (data_stream.is_open())
					{
						data_stream << "D "
							<< inv_sensor_2str(sensorId) << " "
							<< sensorIdhexa << " "
							<< event.status << " "
							<< event.timestamp << " "
							<< orientation.data[0] << " "
							<< orientation.data[1] << " "
							<< orientation.data[2] << " "
							<< orientation.data[3] << " "
							<< int(event.data.quaternion.accuracy_flag) << std::endl;
					}
				}
				break;

			case INV_SENSOR_TYPE_ACCELEROMETER:
				if (m_datafile && m_logacc){
					if (data_stream.is_open())
					{
						data_stream << "D "
							<< inv_sensor_2str(sensorId) << " "
							<< sensorIdhexa << " "
							<< event.status << " "
							<< event.timestamp << " "
							<< event.data.acc.vect[0] << " "
							<< event.data.acc.vect[1] << " "
							<< event.data.acc.vect[2] << " "
							<< int(event.data.acc.accuracy_flag) << std::endl;
					}
				}
				break;

			case INV_SENSOR_TYPE_GYROSCOPE:
				// Wait for the calibration of gyro before starting game rotation
				// vector.
				if (!m_gyro_cal && event.data.gyr.accuracy_flag == 3) {
					m_gyro_cal = true;
					// Enable Game Rotation vector at 1KHz.
					if (m_prediction_enabled && (!m_latencyadding)){
						std::string payload = "2 " + std::to_string((unsigned long long)m_prediction_time);
						m_controller->setSensorConfig(INV_SENSOR_TYPE_PRED_QUAT_1, "pred_quat", payload);
						m_controller->enableSensor(INV_SENSOR_TYPE_PRED_QUAT_1, m_periode);
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
					//osvrDeviceTrackerSendVelocityTimestamped(m_dev, m_tracker, &vel,
					//	0, &timestamp);

					if (m_datafile && m_loggyr){
						if (data_stream.is_open())
						{
							data_stream << "D "
								<< inv_sensor_2str(sensorId) << " "
								<< sensorIdhexa << " "
								<< event.status << " "
								<< event.timestamp << " "
								<< event.data.gyr.vect[0] << " "
								<< event.data.gyr.vect[1] << " "
								<< event.data.gyr.vect[2] << " "
								<< int(event.data.gyr.accuracy_flag) << std::endl;
						}
					}

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
		bool m_gyro_cal, m_datafile, m_logacc, m_loggyr, m_logpq1, m_loggrv,
			m_logracc, m_logrgyr, m_logrmag, m_logrtmp, m_latencyadding;
		unsigned long m_periode;
		long m_prediction_time;
		bool m_prediction_enabled;
		std::string m_fsrGyr, m_fsrAcc;
		int m_buffer_size;
		double m_quat_buffer[1000][4]; // 1s max at 1000Hz
		int m_buffer_pos;
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

			std::string portParam, targetParam, adapterParam, fsrGyr, fsrAcc;
			unsigned long periode;
			long prediction_time;
			bool prediction_enabled, datafile, logacc, loggyr, logpq1, 
				loggrv, logracc, logrgyr, logrmag, logrtmp;

			if (!root.isMember("port")) {
				std::cerr << PREFIX << "Could not find port parameter. Verify that "
					"port is specified in the config file"
					<< std::endl;
				return OSVR_RETURN_FAILURE;
			}
			else {
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
			}
			else {
				targetParam = root["target"].asString();
			}

			if (!root.isMember("adapter")) {
				adapterParam = getAdapterDefault();
				std::cout << PREFIX
					<< "Could not find adapter param. Using default \""
					<< targetParam << "\" parameter" << std::endl;
			}
			else {
				adapterParam = root["adapter"].asString();
			}

			if (!root.isMember("prediction")) {
				prediction_enabled = true;
				prediction_time = 3000;
				std::cout << PREFIX
					<< "Could not find prediction param. Enabling prediction with 3ms " << std::endl;
			}
			else {
				Json::Value& prediction = root["prediction"];
				prediction_enabled = prediction["enabled"].asBool();
				prediction_time = prediction["time"].asFloat();
				std::cout << PREFIX
					<< "Setting up prediction from JSON" << std::endl;
				std::cout << PREFIX
					<< "prediction enabled: " << prediction_enabled << std::endl;
				std::cout << PREFIX
					<< "prediction time: " << prediction_time << std::endl;
			}

			if (!root.isMember("datalog")){
				datafile = false;
			}
			else {
				Json::Value& datalog = root["datalog"];
				datafile = datalog["enabled"].asBool();
				if (datafile) {
					std::string Datafile;
					Datafile = datalog["path"].asString();
					logacc = datalog["Acc"].asBool();
					loggyr = datalog["Gyr"].asBool();
					logpq1 = datalog["PredictiveQuat"].asBool();
					loggrv = datalog["GRV"].asBool();
					logracc = datalog["RawAcc"].asBool();
					logrgyr = datalog["RawGyr"].asBool();
					logrmag = datalog["RawMag"].asBool();
					logrtmp = datalog["RawTmp"].asBool();
					data_stream.open(Datafile.c_str(), std::ostream::ate);
					char mydate[128], mytime[128];
					_strdate_s(mydate);
					_strtime_s(mytime);
					mydate[2] = mydate[5] = mytime[2] = mytime[5] = '-';
					if (data_stream.is_open()){
						std::cout << "Data logging in progress" << std::endl;
						data_stream << "# " << mydate << " " << mytime
							<< " LogAcc:" << logacc
							<< " LogGyr:" << loggyr
							<< " LogPQ1:" << logpq1 
							<< " LogGRV:" << loggrv 
							<< " LogRAcc:" << logracc
							<< " LogRGyr:" << logrgyr
							<< " LogRMag:" << logrmag
							<< " LogRTmp:" << logrtmp
							<< std::endl;
					}
				}
			}

			if (!root.isMember("periode")){
				periode = 1000;
				std::cout << PREFIX
					<< "Could not find periode param. Enabling sensor periode at 1ms " << std::endl;
			}
			else {
				periode = root["periode"].asUInt();
			}
			invensense_DT = 1.0 / periode;

			if (!root.isMember("fsrGyr")){
				fsrGyr = "2000";
				std::cout << PREFIX
					<< "Could not find fsr gyr param. Enabling fsr at 2000 " << std::endl;
			}
			else {
				fsrGyr = root["fsrGyr"].asString();
			}

			if (!root.isMember("fsrAcc")){
				fsrAcc = "8";
				std::cout << PREFIX
					<< "Could not find fsr acc param. Enabling fsr at 8 " << std::endl;
			}
			else {
				fsrAcc = root["fsrAcc"].asString();
			}

			controller = std::shared_ptr<InvenSenseController>(
				new InvenSenseController(targetParam, portParam, adapterParam));

			if (controller->isDeviceConnected()) {
				std::cout << PREFIX << "Detected InvenSense device! " << std::endl;
				m_found = true;

				/// Create our device object
				InvenSenseDevice* pInvnDevice = new InvenSenseDevice(ctx, controller, prediction_time, prediction_enabled, periode, fsrGyr, fsrAcc, 
					datafile, logacc, loggyr, logpq1, loggrv, logracc, logrgyr, logrmag, logrtmp);
				osvr::pluginkit::registerObjectForDeletion(
					ctx, pInvnDevice);
			}
			else {
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

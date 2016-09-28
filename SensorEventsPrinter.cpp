/*
 * ________________________________________________________________________________________________________
 * Copyright (c) 2015-2015 InvenSense Inc. All rights reserved.
 *
 * This software, related documentation and any modifications thereto (collectively “Software”) is subject
 * to InvenSense and its licensors' intellectual property rights under U.S. and international copyright
 * and other intellectual property rights laws.
 *
 * InvenSense and its licensors retain all intellectual property and proprietary rights in and to the Software
 * and any use, reproduction, disclosure or distribution of the Software without an express license agreement
 * from InvenSense is strictly prohibited.
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

#include "SensorEventsPrinter.h"

#include "CommandSensor.h"

SensorEventsPrinter * SensorEventsPrinter::_instance = 0;

bool SensorEventsPrinter::configure(int sensor, const std::string & desc)
{
	// invalid sensor
	if(sensor == -1) {
		return false;
	}

	// decoder affects normal and wake-up
	sensor &= ~INV_SENSOR_TYPE_WU_FLAG;

	// as configure() and print() may be called from a different thread, use a mutex
	Poco::Mutex::ScopedLock lock(_decoders_mutex);

	// descriptor not empty, update decoder
	if(desc.size()) {
		return _decoders[sensor].setDescriptor(desc);
	} else {
		// empty descriptor, remove decoder
		_decoders.erase(sensor);
	}

	return true;
}

void SensorEventsPrinter::print(const inv_sensor_event_t & event, FILE * stream)
{
	std::map<int,BinaryDecoder>::const_iterator it;

	// as configure() and print() may be called from a different thread, use a mutex
	Poco::Mutex::ScopedLock lock(_decoders_mutex);

	it = _decoders.find(event.sensor & ~INV_SENSOR_TYPE_WU_FLAG);

	if(it != _decoders.end()) {
		BinaryDecoder::ValueVect values;

		// decode data
		it->second.decode(event.data.reserved, sizeof(event.data.reserved),
				values, true);

		// print data
		for(unsigned int i = 0; i < values.size(); ++i) {
			fprintf(stream, "%lf ", values[i].val.f);
		}
	} else {
		// print data using default printer function
		printDefault(event, stream);
	}
}

void SensorEventsPrinter::printDefault(const inv_sensor_event_t & event, FILE * stream)
{
	switch(event.sensor & ~INV_SENSOR_TYPE_WU_FLAG) {
	case INV_SENSOR_TYPE_RESERVED:
		break;
	case INV_SENSOR_TYPE_ACCELEROMETER:
	case INV_SENSOR_TYPE_GRAVITY:
	case INV_SENSOR_TYPE_LINEAR_ACCELERATION:
		fprintf(stream, "%f %f %f %d", event.data.acc.vect[0], event.data.acc.vect[1],
				event.data.acc.vect[2], event.data.acc.accuracy_flag);
		break;
	case INV_SENSOR_TYPE_RAW_ACCELEROMETER:
	case INV_SENSOR_TYPE_RAW_GYROSCOPE:
	case INV_SENSOR_TYPE_RAW_MAGNETOMETER:
	case INV_SENSOR_TYPE_HIGH_RATE_GYRO:
		fprintf(stream, "%d %d %d", event.data.raw3d.vect[0], event.data.raw3d.vect[1],
				event.data.raw3d.vect[2]);
		break;
	case INV_SENSOR_TYPE_RAW_TEMPERATURE:
		fprintf(stream, "%d ( %#08x )", event.data.rawtemp.raw,	event.data.rawtemp.raw);
		break;
	case INV_SENSOR_TYPE_RAW_PPG:
		fprintf(stream, "%u %hhu", event.data.rawppg.ppg_value, event.data.rawppg.touch_status);
		break;
	case INV_SENSOR_TYPE_HRV:
		fprintf(stream, "%hhu %hd %hd %hd %hd", event.data.hrv.rr_count, event.data.hrv.rr_interval[0],
				event.data.hrv.rr_interval[1], event.data.hrv.rr_interval[2], 
				event.data.hrv.rr_interval[3]);
		break;
	case INV_SENSOR_TYPE_SLEEP_ANALYSIS:
		fprintf(stream, "%hhu %u %d %d %u %u %hhu", event.data.sleepanalysis.sleep_phase, 
				event.data.sleepanalysis.timestamp, event.data.sleepanalysis.sleep_onset, 
				event.data.sleepanalysis.sleep_latency, event.data.sleepanalysis.time_in_bed,
				event.data.sleepanalysis.total_sleep_time , event.data.sleepanalysis.sleep_efficiency);
		break;
	case INV_SENSOR_TYPE_BAC_STATISTICS:
		fprintf(stream, "%u %u %u %u %u %u %u %u %u %u %u", event.data.bacstat.durationWalk,
				event.data.bacstat.durationRun, event.data.bacstat.durationTransportSit,
				event.data.bacstat.durationTransportStand, event.data.bacstat.durationBiking,
				event.data.bacstat.durationStillSit, event.data.bacstat.durationStillStand,
				event.data.bacstat.durationTotalSit, event.data.bacstat.durationTotalStand,
				event.data.bacstat.stepWalk, event.data.bacstat.stepRun);
		break;
	case INV_SENSOR_TYPE_FLOOR_CLIMB_COUNTER:
		fprintf(stream, "%u %u", event.data.floorclimb.floorsUp, event.data.floorclimb.floorsDown);
		break;
	case INV_SENSOR_TYPE_ENERGY_EXPENDITURE:
		fprintf(stream, "%u %u %u %u", event.data.energyexp.instantEEkcal,
				event.data.energyexp.instantEEmets, event.data.energyexp.cumulativeEEkcal,
				event.data.energyexp.cumulativeEEmets);
		break;
	case INV_SENSOR_TYPE_DISTANCE:
		fprintf(stream, "%u %u", event.data.distance.distanceWalk, event.data.distance.distanceRun);
		break;
	case INV_SENSOR_TYPE_MAGNETOMETER:
		fprintf(stream, "%f %f %f %d", event.data.mag.vect[0], event.data.mag.vect[1],
				event.data.mag.vect[2], event.data.mag.accuracy_flag);
		break;
	case INV_SENSOR_TYPE_DATA_ENCRYPTION: 
		fprintf(stream, "%d %d %d %d %d %d %d", event.data.dataencryption.table[0], event.data.dataencryption.table[1], event.data.dataencryption.table[2],
				event.data.dataencryption.table[3], event.data.dataencryption.table[4], event.data.dataencryption.table[5], event.data.dataencryption.table[6]);
		break;
	case INV_SENSOR_TYPE_GYROSCOPE:
		fprintf(stream, "%f %f %f %d", event.data.gyr.vect[0], event.data.gyr.vect[1],
				event.data.gyr.vect[2], event.data.gyr.accuracy_flag);
		break;
	case INV_SENSOR_TYPE_ORIENTATION:
		fprintf(stream, "%f %f %f %d", event.data.orientation.x, event.data.orientation.y,
				event.data.orientation.z, event.data.orientation.accuracy_flag);
		break;
	case INV_SENSOR_TYPE_UNCAL_MAGNETOMETER:
		fprintf(stream, "%f %f %f %f %f %f %d", event.data.mag.vect[0], event.data.mag.vect[1],
				event.data.mag.vect[2], event.data.mag.bias[0], event.data.mag.bias[1],
				event.data.mag.bias[2], event.data.mag.accuracy_flag);
		break;
	case INV_SENSOR_TYPE_UNCAL_GYROSCOPE:
		fprintf(stream, "%f %f %f %f %f %f %d", event.data.gyr.vect[0], event.data.gyr.vect[1],
				event.data.gyr.vect[2], event.data.gyr.bias[0], event.data.gyr.bias[1],
				event.data.gyr.bias[2], event.data.gyr.accuracy_flag);
		break;
	case INV_SENSOR_TYPE_STEP_COUNTER:
		fprintf(stream, "%llu", event.data.step.count);
		break;
	case INV_SENSOR_TYPE_ROTATION_VECTOR:
	case INV_SENSOR_TYPE_GEOMAG_ROTATION_VECTOR:
	case INV_SENSOR_TYPE_GAME_ROTATION_VECTOR:
	case INV_SENSOR_TYPE_3AXIS:
		fprintf(stream, "%f %f %f %f %f %d", event.data.quaternion.quat[0],
				event.data.quaternion.quat[1], event.data.quaternion.quat[2],
				event.data.quaternion.quat[3], event.data.quaternion.accuracy,
				event.data.quaternion.accuracy_flag);
		break;
	case INV_SENSOR_TYPE_SMD:
	case INV_SENSOR_TYPE_STEP_DETECTOR:
	case INV_SENSOR_TYPE_TILT_DETECTOR:
	case INV_SENSOR_TYPE_WAKE_GESTURE:
	case INV_SENSOR_TYPE_GLANCE_GESTURE:
	case INV_SENSOR_TYPE_PICK_UP_GESTURE:
	case INV_SENSOR_TYPE_B2S:
	case INV_SENSOR_TYPE_SHAKE:
	case INV_SENSOR_TYPE_DOUBLE_TAP:
	case INV_SENSOR_TYPE_SEDENTARY_REMIND:
		fprintf(stream, "%d", event.data.event);
		break;
	case INV_SENSOR_TYPE_WOM:
		fprintf(stream, "0x%02x", event.data.wom.flags);
		fprintf(stream, " %c", (event.data.wom.flags & 0x01) ? 'X' : ' ');
		fprintf(stream, " %c", (event.data.wom.flags & 0x02) ? 'Y' : ' ');
		fprintf(stream, " %c", (event.data.wom.flags & 0x04) ? 'Z' : ' ');
		break;
	case INV_SENSOR_TYPE_BAC:
		if(event.data.bac.event)
			fprintf(stream, "%d - %-5s '%s'",
					event.data.bac.event,
					(event.data.bac.event > 0 ? "BEGIN" : "END"),
					activityName(event.data.bac.event));
		else
			fprintf(stream, "0 - UNKNOWN");
		break;
	case INV_SENSOR_TYPE_BAC_EXTENDED:
		if(event.data.bac.event)
			fprintf(stream, "%d - %-5s '%s'",
					event.data.bacext.event,
					(event.data.bacext.event > 0 ? "BEGIN" : "END"),
					activityExtName(event.data.bacext.event));
		else
			fprintf(stream, "0 - UNKNOWN");
		break;
	case INV_SENSOR_TYPE_PRESSURE:
		fprintf(stream, "%d", event.data.pressure.pressure);
		break;
	case INV_SENSOR_TYPE_TEMPERATURE:
	case INV_SENSOR_TYPE_AMBIENT_TEMPERATURE:
		fprintf(stream, "%f", event.data.temperature.tmp);
		break;
	case INV_SENSOR_TYPE_LIGHT:
		fprintf(stream, "%u", event.data.light.level);
		break;
	case INV_SENSOR_TYPE_PROXIMITY:
		fprintf(stream, "%u", event.data.proximity.distance);
		break;
	case INV_SENSOR_TYPE_HUMIDITY:
		fprintf(stream, "%f", event.data.humidity.percent);
		break;
	case INV_SENSOR_TYPE_HEART_RATE:
		fprintf(stream, "%f %hhu %hhu", event.data.hrm.bpm, event.data.hrm.confidence, event.data.hrm.sqi);
		break;
	case INV_SENSOR_TYPE_PDR:
		for(unsigned int i = 0; i < sizeof(event.data.pdr.fxdata)/sizeof(event.data.pdr.fxdata); ++i) {
			fprintf(stream, "%f ", (event.data.pdr.fxdata[i] / (float)(1 << 15)) );
		}
		break;

	case INV_SENSOR_TYPE_OIS:
		fprintf(stream, "%d %d %d", event.data.raw3d.vect[0], event.data.raw3d.vect[1], event.data.raw3d.vect[2]);
		break;

	case INV_SENSOR_TYPE_EIS:
		fprintf(stream, "%f %f %f %f %f %f %d", event.data.eis.vect[0], event.data.eis.vect[1],
				event.data.eis.vect[2], event.data.eis.bias[0], event.data.eis.bias[1],
				event.data.eis.bias[2], event.data.eis.delta_ts);
		break;

	case INV_SENSOR_TYPE_KWR:
		fprintf(stream, "audio buffer addr= %p size= %u",
				event.data.audio_buffer.buffer, event.data.audio_buffer.size);
		break;

	case INV_SENSOR_TYPE_TSIMU:
		fprintf(stream, "%d %d %d %d %d %d", 
				event.data.tsimu_status.status[0],
				event.data.tsimu_status.status[1],
				event.data.tsimu_status.status[2],
				event.data.tsimu_status.status[3],
				event.data.tsimu_status.status[4],
				event.data.tsimu_status.status[5]);
		break;

	default:
		for(unsigned int i = 0; i < sizeof(event.data.reserved); ++i) {
			fprintf(stream, "0x%02x ", event.data.reserved[i]);
		}
		break;
	}
}

const char * SensorEventsPrinter::activityName(int act)
{
	switch(abs(act)) {
	case INV_SENSOR_BAC_EVENT_ACT_IN_VEHICLE_BEGIN:          return "IN_VEHICLE";
	case INV_SENSOR_BAC_EVENT_ACT_WALKING_BEGIN:             return "WALKING";
	case INV_SENSOR_BAC_EVENT_ACT_RUNNING_BEGIN:             return "RUNNING";
	case INV_SENSOR_BAC_EVENT_ACT_ON_BICYCLE_BEGIN:          return "ON_BICYCLE";
	case INV_SENSOR_BAC_EVENT_ACT_TILT_BEGIN:                return "TILT";
	case INV_SENSOR_BAC_EVENT_ACT_STILL_BEGIN:               return "STILL";
	default:                                                 return "UNKNOWN";
	}
}

const char * SensorEventsPrinter::activityExtName(int act)
{
	switch(abs(act)) {
	case INV_SENSOR_BACEXT_EVENT_ACT_WALKING_START:          return "WALKING";
	case INV_SENSOR_BACEXT_EVENT_ACT_RUNNING_START:          return "RUNNING";
	case INV_SENSOR_BACEXT_EVENT_ACT_ON_BICYCLE_START:       return "ON_BICYCLE";
	case INV_SENSOR_BACEXT_EVENT_ACT_IN_VEHICLE_SIT_START:   return "IN_VEHICLE_SIT";
	case INV_SENSOR_BACEXT_EVENT_ACT_IN_VEHICLE_STAND_START: return "IN_VEHICLE_STAND";
	case INV_SENSOR_BACEXT_EVENT_ACT_STILL_SIT_START:        return "STILL_SIT";
	case INV_SENSOR_BACEXT_EVENT_ACT_STILL_STAND_START:      return "STILL_STAND";
	default:                                                 return "UNKNOWN";
	}
}

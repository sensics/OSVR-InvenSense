/*
 * ________________________________________________________________________________________________________
 * Copyright © 2015 InvenSense Inc.  All rights reserved.
 *
 * This software and/or documentation  (collectively “Software”) is subject to InvenSense intellectual property rights
 * under U.S. and international copyright and other intellectual property rights laws.
 *
 * The Software contained herein is PROPRIETARY and CONFIDENTIAL to InvenSense and is provided
 * solely under the terms and conditions of a form of InvenSense software license agreement between
 * InvenSense and you and any use, modification, reproduction or disclosure of the Software without
 * such agreement or the express written consent of InvenSense is strictly prohibited.
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

#include <cmath>

class SensorEventsTimestampStats {
public:
	SensorEventsTimestampStats()
	{
		reset();
	}

	void reset()
	{
		_cnt  = 0;
		_sum  = 0;
		_sum2 = 0;
		_min  = 0;
		_max  = 0;
		_dt   = 0;
		_last_timestamp = 0;
	}

	void update(uint64_t timestamp)
	{
		if(_cnt != 0) {
			_dt = timestamp - _last_timestamp;
			_sum  += _dt;
			_sum2 += _dt*_dt;

			if(_dt < _min || _cnt == 1) {
				_min = _dt;
			}

			if(_dt > _max || _cnt == 1) {
				_max = _dt;
			}
		}
		_last_timestamp = timestamp;
		_cnt  += 1;
	}

	uint64_t getCnt() const { return _cnt; }
	uint64_t getDt() const  { return _dt; }
	uint64_t getMin() const { return _min; }
	uint64_t getMax() const { return _max; }

	float getAverage() const
	{
		if(_cnt > 1) {
			return (float)_sum / (_cnt - 1);
		} else {
			return (float)_dt;
		}
	}

	float getStd() const
	{
		if(_cnt > 1) {
			const float average  = getAverage();
			const float average2 = (float)_sum2 / (_cnt - 1);
			return sqrt(abs(average2 - average*average));
		} else {
			return 0;
		}
	}

private:
	uint64_t _cnt;
	uint64_t _dt;
	uint64_t _sum;
	uint64_t _sum2;
	uint64_t _min, _max;
	uint64_t _last_timestamp;
};

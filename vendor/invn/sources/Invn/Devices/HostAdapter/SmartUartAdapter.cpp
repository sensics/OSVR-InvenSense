/*
 * ________________________________________________________________________________________________________
 * Copyright (c) 2016-2016 InvenSense Inc. All rights reserved.
 *
 * This software, related documentation and any modifications thereto (collectively "Software" is subject
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

#include "UartHostAdapter.h"
#include "SmartUartAdapter.h"
#include "Ft232UartAdapter.h"
#ifdef _WIN32
	#include "WinUartAdapter.h"
#else
	#include "LinUartAdapter.h"
#endif

#include <string>


SmartUartAdapter::SmartUartAdapter(const std::string & port)
{
#ifdef _WIN32
	std::string com_port = port;
	/* If COM port not specified, try to find it automatically. */
	/* It will succed only if ONE ft232r is connected and not opened by other threads. */
	if(com_port.empty()) {
		com_port = Ft232UartAdapter::getFt232Port();
	}

	/* Now that we have a port, build the UartHostAdapter instance */
	if (Ft232UartAdapter::isFt232Available(com_port)) {
		_impl = new Ft232UartAdapter(com_port);
	} else {
		_impl = new WinUartAdapter(com_port);
	}
#else
	_impl = new LinUartAdapter(port);
#endif
}

SmartUartAdapter::~SmartUartAdapter()
{
	delete _impl;
}

void SmartUartAdapter::open()
{
	_impl->open();
}

void SmartUartAdapter::close()
{
	_impl->close();
}

int SmartUartAdapter::read(unsigned char * data, unsigned len)
{
	return _impl->read(data, len);
}

int SmartUartAdapter::write(const unsigned char * data, unsigned len)
{
	return _impl->write(data, len);
}

void SmartUartAdapter::setBaudrate(const unsigned int baud)
{
	_impl->setBaudrate(baud);
}

void SmartUartAdapter::setFlowcontrol(UartHostAdapterFlowcontrol flow)
{
	_impl->setFlowcontrol(flow);
}

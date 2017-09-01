/*
 * ________________________________________________________________________________________________________
 * Copyright (c) 2016-2016 InvenSense Inc. All rights reserved.
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

#ifndef _INV_SMART_UART_ADAPTER_H_
#define _INV_SMART_UART_ADAPTER_H_

#include "Invn/InvExport.h"
#include "UartHostAdapter.h"

/** @brief Proxy that delegates either to Ft232Adapter or to WinUartAdapter object depending on the 
  *        posibilities of the physical device.
  */
class INV_EXPORT SmartUartAdapter: public UartHostAdapter
{
public:
	/** @brief Builds the underlying implementation object depending on the physical device connected 
	  *        and on the operating system.
	  *        If the operating system is Windows and an ft232r is connected, an Ft232UartAdapter object 
	  *        will be instantiated. If only one ft232r is connected and  it is not opened, 
	  *        'port' can be an empty string because the COM port address will be automatically detected. 
	  *        If multiple ft232r devices are connected, then the COM port needs to be specified.
	  *        If the operating system is not Windows, a LinUartAdapter object will be instantiated.
	  */
	SmartUartAdapter(const std::string & port);
	~SmartUartAdapter();

	/** @brief Delegated to the implementation.
	  */
	void open();

	/** @brief Delegated to the implementation.
	  */
	void close();

	/** @brief Delegated to the implementation.
	  */
	int read(unsigned char * data, unsigned len);
	
	/** @brief Delegated to the implementation.
	  */
	int write(const unsigned char * data, unsigned len);
	
	/** @brief Delegated to the implementation.
	  */
	void setBaudrate(const unsigned int baud);
	
	/** @brief Delegated to the implementation.
	  */
	void setFlowcontrol(UartHostAdapterFlowcontrol flow);

private:
	UartHostAdapter *    _impl;
};

#endif /* _INV_SMART_UART_ADAPTER_H_ */

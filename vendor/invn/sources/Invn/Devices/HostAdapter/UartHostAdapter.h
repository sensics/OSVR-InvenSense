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

#ifndef _INV_UART_HOST_ADAPTER_H_
#define _INV_UART_HOST_ADAPTER_H_

#include "Invn/InvExport.h"
#include "HostAdapter.h"

#include <stdint.h>

/** @brief Abstract interface to UART adapters
 *
 *  Provides a uniform way of communication to UART devices through an adapter connected
 *  to a host (such as FT232R, EDBG, etc.)
 */
class INV_EXPORT UartHostAdapter : public HostAdapter
{
public:
	enum UartHostAdapterFlowcontrol {
		INVN_UART_HOST_ADAPTER_FLOW_NONE = 0,
		INVN_UART_HOST_ADAPTER_FLOW_XON_XOFF,
		INVN_UART_HOST_ADAPTER_FLOW_RTS_CTS,
		INVN_UART_HOST_ADAPTER_FLOW_DTR_DSR,
	};

	/** @brief Open and initialize adapter
	 *  @throw OpenExcept
	 */
	virtual void open() = 0;

	/** @brief Close adapter
	 */
	virtual void close() = 0;

	/** @brief Read bytes from the UART device
	 *  @param[out] data   pointer to a buffer were the read data will be stored
	 *  @param[in]  len    number of bytes to read
	 *  @throw TransportExcept
	 */
	virtual int read(unsigned char * data, unsigned len) = 0;

	/** @brief Write bytes to the UART device
	 *  @param[in]  data   pointer to buffer containig the data to be written
	 *  @param[in]  len    number of bytes to write
	 *  @throw TransportExcept
	 */
	virtual int write(const unsigned char * data, unsigned len) = 0;

	/** @brief Set the baud rate.
	  */
	virtual void setBaudrate(const unsigned int baud) { (void)baud; };

	/** @brief Set the flow control.
	  */
	virtual void setFlowcontrol(UartHostAdapterFlowcontrol flow) { (void)flow; };

	/** @brief Return the type of underlying serial interface
	 */
	HostAdapter::SerifType getSerifType() const;

	/** @brief Virtual destructor
	 */
	virtual ~UartHostAdapter();

private:
	void readReg(uint8_t reg, uint8_t * data, uint32_t len) { (void)reg; (void)data; (void)len; };
	void writeReg(uint8_t reg, const uint8_t * data, uint32_t len) { (void)reg; (void)data; (void)len; };
	uint32_t getMaxReadTransactionSize() const {return 0;};
	uint32_t getMaxWriteTransactionSize() const {return 0;};
};

#endif /* _INV_UART_HOST_ADAPTER_H_ */

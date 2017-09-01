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

/** @defgroup HostAdapter Host Adapter abstract Interface
 *	@brief    Virtual abstraction of host adapter for serial interface
 *	@{
 */

#ifndef _INV_HOST_ADAPTER_H_
#define _INV_HOST_ADAPTER_H_

#include "Invn/InvExport.h"

#include <stdint.h>
#include <string>
#include <exception>

/** @brief Abstract interface to host adapter
 *
 *  Provides an uniform way to communicate to device through an adapter connected
 *  to a host (such as Cheetah, Aardvark, FTDI, ...)
 */
class INV_EXPORT HostAdapter
{
public:

	/** @brief Singleton class for HostAdapter
 	 */
	class Singleton
	{
	public:
		/* @brief Return singleton instance of the HostAdapter
		 */
		static HostAdapter * getInstance();
		/* @brief Set singleton instance of the HostAdapter
		 */
		static void setInstance(HostAdapter * inst);
	private:
		Singleton()  {}
		~Singleton() {}
		static HostAdapter * _instance;
	};

	/** @brief Helper macro to declare a HostAdater exception
	 */
	#define HOST_ADAPTER_DECLARE_EXCEPT(_Exc, _Base) \
		struct _Exc : public _Base { virtual const char *what() const throw(); };

	/** @brief Unspecified HostAdater exception definition
	 */
	HOST_ADAPTER_DECLARE_EXCEPT(Except,          std::exception)

	/** @brief "No Found" HostAdater exception definition
	 *  Thrown when attempting to open an unconnected host adapter
	 */
	HOST_ADAPTER_DECLARE_EXCEPT(NotFoundExcept,  HostAdapter::Except)

	/** @brief "Open" HostAdater exception definition
	 *  Thrown when an error occures when openning a host adapter
	 */
	HOST_ADAPTER_DECLARE_EXCEPT(OpenExcept,      HostAdapter::Except)

	/** @brief "Transport" HostAdater exception definition
	 *  Thrown when an error occures when performing a serial read or write transaction
	 */
	HOST_ADAPTER_DECLARE_EXCEPT(TransportExcept, HostAdapter::Except)

	/** @brief "Config" HostAdater exception definition
	 *  Thrown when attemting to set a configuration with a bad or unexpected value
	 */
	HOST_ADAPTER_DECLARE_EXCEPT(ConfigExcept,    HostAdapter::Except)

	/** @brief "Size" HostAdater exception definition
	 *  Thrown when attemting perform a read or write transaction with a payload
	 *  greater than the maximum transaction size value
	 */
	HOST_ADAPTER_DECLARE_EXCEPT(SizeExcept,      HostAdapter::Except)

	#undef HOST_ADAPTER_DECLARE_EXCEPT

	/** @brief Serial Interface type definition
 	 */
	enum SerifType {
		SERIF_TYPE_NONE, /**< unspecified serial interface */
		SERIF_TYPE_I2C,  /**< I2C serial interface */
		SERIF_TYPE_SPI,  /**< SPI serial interface */
		SERIF_TYPE_UART
	};

	/** @brief Get type of underlying serial interface
	 */
	virtual SerifType getSerifType() const = 0;

	/** @brief Open connection to and initialize adapter
	 *	@throw NotFoundExcept, OpenExcept
	 */
	virtual void open() = 0;

	/** @brief Close connection to Serial Interface adapter
	 */
	virtual void close() = 0;

	/** @brief Perform a read register transaction over the serial interface
	 *	@param[in]	reg    register
	 *	@param[out]	data   pointer to output buffer
	 *	@param[in]  len    number of byte to read
	 *	@throw SizeExcept (if len > getMaxTransactionSize()), TransportExcept
	 */
	virtual void readReg(uint8_t reg, uint8_t * data, uint32_t len) = 0;

	/** @brief Perform a write register transaction over the serial interface
	 *	@param[in]	reg    register
	 *	@param[out]	data   pointer to output buffer
	 *	@param[in]  len    number of byte to write
	 *	@throw SizeExcept (if len > getMaxTransactionSize()), TransportExcept
	 */
	virtual void writeReg(uint8_t reg, const uint8_t * data, uint32_t len) = 0;

	/** @brief Get maximum number of bytes allowed per serial read transaction
	 */
	virtual uint32_t getMaxReadTransactionSize() const = 0;

	/** @brief Get maximum number of bytes allowed per serial write transaction
	 */
	virtual uint32_t getMaxWriteTransactionSize() const = 0;

	/** @brief Set configuration value for host adapter
	 *
	 *  Must be called before open()
	 *
	 *	@param[in]	name   config option name
	 *	@param[out]	data   config option value
	 *	@param[in]  len    number of byte to write
	 *	@throw ConfigExcept if option value is incorrect
	 *  @return true if option name is correct
	 *          false if option name is unknown and was ignored
	 */
	virtual bool setConfig(const std::string & name, const std::string & value);

	/** @brief Helper to set singleton instance
	 */
	void setSingletonInstance()
	{
		Singleton::setInstance(this);
	}

	/** @brief Virtual desctrutor
	 */
	virtual ~HostAdapter();
};

#endif /* _INV_HOST_ADAPTER_H_ */

/** @} */

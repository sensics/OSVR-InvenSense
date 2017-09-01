//-----------------------------------------------------------------------------
/*
 Copyright © 2015 InvenSense Inc. All rights reserved.

 This software, related documentation and any modifications thereto collectively “Software” is subject
 to InvenSense and its licensors' intellectual property rights under U.S. and international copyright and
 other intellectual property rights laws.

 InvenSense and its licensors retain all intellectual property and proprietary rights in and to the Software
 and any use, reproduction, disclosure or distribution of the Software without an express license
 agreement from InvenSense is strictly prohibited.
 */
//-----------------------------------------------------------------------------

#include "HostAdapterClient.h"

#include "Invn/Devices/HostAdapter/AardvarkI2cAdapter.h"
#include "Invn/Devices/HostAdapter/CheetahAdapter.h"
#include "Invn/Devices/HostAdapter/DummyAdapter.h"
#include "Invn/Devices/HostAdapter/Ft4222SpiAdapter.h"
#include "Invn/Devices/HostAdapter/HostAdapter2SerifHal.h"
#include "Invn/Devices/HostAdapter/UartSpiBridgeAdapter.h"

#include "Invn/EmbUtils/ErrorHelper.h"
#include "Invn/HostUtils/OptParser.h"

#include <stdexcept>
#include <limits>
#include <memory>

#define IMPL_EXCEPTION(_Exc, _msg) \
	const char* _Exc::what() const throw() { return _msg; }

IMPL_EXCEPTION(HostAdapterClient::OpenException, "Cannot open the device")
IMPL_EXCEPTION(HostAdapterClient::NotFoundException, "Cannot find the device")
IMPL_EXCEPTION(HostAdapterClient::ProtocolException, "Cannot communicate with the device")
IMPL_EXCEPTION(HostAdapterClient::UnknowOptionException, "Unknown option '")

//-----------------------------------------------------------------------------

HostAdapterClient::HostAdapterClient(HostAdapter * adapter)
: _handle(adapter)
, _opened(false)
{
	HostAdapter2SerifHal(_handle, &_serif);
	adapter->setSingletonInstance();
	buildHostSerif();
}

//-----------------------------------------------------------------------------

HostAdapterClient::~HostAdapterClient()
{
	close();
	delete _handle;
	HostAdapter::Singleton::setInstance(0);
}

//-----------------------------------------------------------------------------

// create a host serif object that calls HostAdapter singleton instance for legacy purpose

static int host_serif_open(void)
{
	try {
		HostAdapter::Singleton::getInstance()->open();
	}
	catch(...) {
		return -1;
	}

	return 0;
}

static int host_serif_close(void)
{
	try {
		HostAdapter::Singleton::getInstance()->close();
	}
	catch(...) {
		return -1;
	}

	return 0;
}

static int host_serif_read_reg(uint8_t reg, uint8_t * data, uint32_t len)
{
	try {
		HostAdapter::Singleton::getInstance()->readReg(reg, data, len);
	}
	catch(...) {
		return -1;
	}

	return 0;
}

static int host_serif_write_reg(uint8_t reg, const uint8_t * data, uint32_t len)
{
	try {
		HostAdapter::Singleton::getInstance()->writeReg(reg, data, len);
	}
	catch(...) {
		return -1;
	}

	return 0;
}

void HostAdapterClient::buildHostSerif()
{
	_host_serif.open      = host_serif_open;
	_host_serif.close     = host_serif_close;
	_host_serif.read_reg  = host_serif_read_reg;
	_host_serif.write_reg = host_serif_write_reg;
	_host_serif.register_interrupt_callback = 0;
	_host_serif.max_read_size = HostAdapter::Singleton::getInstance()->getMaxReadTransactionSize();
	_host_serif.max_write_size = HostAdapter::Singleton::getInstance()->getMaxWriteTransactionSize();

	switch(HostAdapter::Singleton::getInstance()->getSerifType()) {
	case HostAdapter::SERIF_TYPE_I2C:
		_host_serif.serif_type = INV_HOST_SERIF_TYPE_I2C;
		break;
	case HostAdapter::SERIF_TYPE_SPI:
		_host_serif.serif_type = INV_HOST_SERIF_TYPE_SPI;
		break;
	default:
	case HostAdapter::SERIF_TYPE_NONE:
		_host_serif.serif_type = INV_HOST_SERIF_TYPE_NONE;
		break;
	}
}

const inv_host_serif_t* HostAdapterClient::getHandle() const
{
	return &_host_serif;
}

//-----------------------------------------------------------------------------

void HostAdapterClient::open()
{
	if (_opened)
		return;

	try
	{
		_handle->open();
		// update serif hal object as max transaction size value can be changed after open()
		HostAdapter2SerifHal(_handle, &_serif);
		buildHostSerif();
	}
	catch(const HostAdapter::NotFoundExcept &)
	{
		throw NotFoundException();
	}
	catch(const HostAdapter::OpenExcept &)
	{
		throw OpenException();
	}
	catch(const HostAdapter::Except &)
	{
		throw ProtocolException();
	}
	catch(...) {
		throw std::runtime_error("Unknown exception");
	}

	_opened = true;
}

//-----------------------------------------------------------------------------

void HostAdapterClient::close()
{
	if (!_opened)
		return;

	_handle->close();
	_opened = 0;
}

//-----------------------------------------------------------------------------

void HostAdapterClient::setOption(const std::string& sOptionName, int iValue)
{
	(void)sOptionName, (void)iValue;
	throw UnknowOptionException();
}

//-----------------------------------------------------------------------------

void HostAdapterClient::setOption(const std::string& sOptionName, const std::string& sValue)
{
	(void)sOptionName, (void)sValue;
	throw UnknowOptionException();
}

//-----------------------------------------------------------------------------

/// @return true if opened, false if not found.
/// @throw if found but unable to open it.
static bool _tryOpen(HostAdapterClient* pAdapter)
{
	try
	{
		pAdapter->open();
		return true;
	}
	catch (const HostAdapterClient::NotFoundException&)
	{
		return false;
	}
}

template <class THostAdapter>
static HostAdapterClient* _tryOpen()
{
	std::auto_ptr<THostAdapter> pAdapter(new THostAdapter());

	try
	{
		pAdapter->open();
	}
	catch (const HostAdapterClient::NotFoundException&)
	{
		return 0;
	}

	return pAdapter.release();
}

HostAdapterClient* HostAdapterClient::autoSelect()
{
	if (HostAdapterClient* pAdapter = _tryOpen<HostAdapterClientUartSpiBridge>())
		return pAdapter;
	if (HostAdapterClient* pAdapter = _tryOpen<HostAdapterClientAardvark>())
		return pAdapter;
	if (HostAdapterClient* pAdapter = _tryOpen<HostAdapterClientCheetah>())
		return pAdapter;
	if (HostAdapterClient* pAdapter = _tryOpen<HostAdapterClientFt4222Spi>())
		return pAdapter;

	return 0;
}

HostAdapterClient* HostAdapterClient::factoryCreate(const std::string& sAdapterName, const OptionMap& options)
{
	const OptMapParser opt(options);
	std::auto_ptr<HostAdapterClient> pAdapter;

	if(sAdapterName == "dummy")
	{
		pAdapter.reset(new HostAdapterClientDummy());
	}
	else if(sAdapterName == "aardvarki2c")
	{
		int speed;
		unsigned addr, slave_mode;

		HostAdapterClientAardvark* pAardvark = new HostAdapterClientAardvark();
		pAdapter.reset(pAardvark);

		if(Poco::NumberParser::tryParse(opt.get("speed"), speed))
			pAardvark->setSpeedOption(static_cast<uint16_t>(speed));

		if(Poco::NumberParser::tryParseHex(opt.get("addr"), addr))
			pAardvark->setAddrOption(static_cast<uint8_t>(addr));

		if(Poco::NumberParser::tryParseHex(opt.get("slavemode"), slave_mode))
			pAardvark->setSlaveModeOption(static_cast<uint8_t>(slave_mode));

	}
	else if(sAdapterName == "cheetah")
	{
		int speed, uid;
		HostAdapterClientCheetah* pCheetah = new HostAdapterClientCheetah();
		pAdapter.reset(pCheetah);

		if(Poco::NumberParser::tryParse(opt.get("speed"), speed))
			pCheetah->setSpeedOption(static_cast<uint16_t>(speed));

		if(Poco::NumberParser::tryParse(opt.get("uid"), uid))
			pCheetah->setUidOption(static_cast<uint32_t>(uid));

	}
	else if(sAdapterName == "spibridge" || sAdapterName == "arduino" || sAdapterName == "nucleo")
	{
		HostAdapterClientUartSpiBridge* pSpiBridge = new HostAdapterClientUartSpiBridge();
		pAdapter.reset(pSpiBridge);

		const std::string& sPort = opt.get("port");
		if(sPort != "")
			pSpiBridge->setComPort(sPort);

	}
	else if(sAdapterName == "ft4222spi" || sAdapterName == "ftdispi")
	{
		HostAdapterClientFt4222Spi* pFt4222 = new HostAdapterClientFt4222Spi();
		pAdapter.reset(pFt4222);

		int speed;
		if(Poco::NumberParser::tryParse(opt.get("speed"), speed))
			pFt4222->setSpeedOption(static_cast<uint16_t>(speed));
	}

	return pAdapter.release();
}

//-----------------------------------------------------------------------------

// TODO: implement them in other files
// TODO: implement setOption in these class in order to be generic

HostAdapterClientUartSpiBridge::HostAdapterClientUartSpiBridge() : HostAdapterClient(new UartSpiBridgeAdapter()) { }
const char* HostAdapterClientUartSpiBridge::getName() const { return "SPI_Bridge"; }
void HostAdapterClientUartSpiBridge::setComPort(const std::string& port) {
	reinterpret_cast<UartSpiBridgeAdapter *>(_handle)->setComPort(port);
}

//-----------------------------------------------------------------------------

HostAdapterClientAardvark::HostAdapterClientAardvark() : HostAdapterClient(new AardvarkI2cAdapter()) { }
const char* HostAdapterClientAardvark::getName() const { return "Aardvark"; }
void HostAdapterClientAardvark::setSpeedOption(uint16_t clock) {
	reinterpret_cast<AardvarkI2cAdapter *>(_handle)->setClock(clock);
}
void HostAdapterClientAardvark::setAddrOption(uint8_t addr) {
	reinterpret_cast<AardvarkI2cAdapter *>(_handle)->setAddr(addr);
}
void HostAdapterClientAardvark::setSlaveModeOption(uint8_t addr) {
	reinterpret_cast<AardvarkI2cAdapter *>(_handle)->setSlaveMode(addr);
}

//-----------------------------------------------------------------------------

HostAdapterClientCheetah::HostAdapterClientCheetah() : HostAdapterClient(new CheetahAdapter()) { }
const char* HostAdapterClientCheetah::getName() const { return "Cheetah"; }
void HostAdapterClientCheetah::setSpeedOption(uint16_t clock) {
	reinterpret_cast<CheetahAdapter *>(_handle)->setClock(clock);
}
void HostAdapterClientCheetah::setUidOption(uint32_t uid) {
	reinterpret_cast<CheetahAdapter *>(_handle)->setUID(uid);
}

//-----------------------------------------------------------------------------

HostAdapterClientFt4222Spi::HostAdapterClientFt4222Spi() : HostAdapterClient(new Ft4222SpiAdapter()) { }
const char* HostAdapterClientFt4222Spi::getName() const { return "FT4222 SPI"; }
void HostAdapterClientFt4222Spi::setSpeedOption(uint16_t clock) {
	reinterpret_cast<Ft4222SpiAdapter *>(_handle)->setClock(clock);
}

//-----------------------------------------------------------------------------

HostAdapterClientDummy::HostAdapterClientDummy() : HostAdapterClient(new DummyAdapter()) { }
const char* HostAdapterClientDummy::getName() const { return "Dummy"; }

//-----------------------------------------------------------------------------


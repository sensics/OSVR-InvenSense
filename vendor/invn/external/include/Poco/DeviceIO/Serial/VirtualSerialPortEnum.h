//-----------------------------------------------------------------------------
/*
 (c) Copyright 2010 Movea, S.A., unpublished work.
 This computer program includes Confidential, Proprietary Information
 and is a trade secret of Movea. All use, disclosure, and/or
 reproduction is prohibited unless authorized in writing by an officer
 of Movea. All Rights Reserved.
 */
//-----------------------------------------------------------------------------

#ifndef VIRTUALSERIALPORTENUM_H_INCLUDED
#define VIRTUALSERIALPORTENUM_H_INCLUDED

// STL
#include <memory>
#include <vector>
#include <string>

namespace Poco {
namespace DeviceIO {
namespace Serial {

//-----------------------------------------------------------------------------

/// Enumerate all USB devices that have a virtual serial port
class VirtualSerialPortEnum
{
public:

	struct DeviceInfo
	{
		DeviceInfo() : opened(false) { }
		virtual ~DeviceInfo() { }

		std::string portName;
		std::string serialNumber;
		bool opened;
	};

	typedef std::vector<DeviceInfo> DeviceMap;
	typedef DeviceMap::const_iterator Iterator;
	typedef DeviceMap::const_iterator ConstIterator;
	typedef DeviceMap::const_iterator iterator;
	typedef DeviceMap::const_iterator const_iterator;

	/// Constructor
	/// Retrieve all serial ports associated to the USB device with the @p pid and @p vid signature
	/// The @p manufacturerName and @p productName are also required to check found devices are matching
	VirtualSerialPortEnum(int vid, int pid, const std::string& manufacturerName, const std::string& productName);

	/// Destructor
	virtual ~VirtualSerialPortEnum();

	Iterator begin() const;
	Iterator end() const;
	int size() const;
	const DeviceInfo& operator[] (int index) const;

private:
	DeviceMap _devices;
};

//-----------------------------------------------------------------------------

} } } // namespace Poco::DeviceIO::Serial

#endif /* VIRTUALSERIALPORTENUM_H_INCLUDED */

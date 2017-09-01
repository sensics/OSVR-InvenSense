//-----------------------------------------------------------------------------
/*
     (c) Copyright 2008 Movea, S.A., unpublished work.
     This computer program includes Confidential, Proprietary Information
     and is a trade secret of Movea. All use, disclosure, and/or
     reproduction is prohibited unless authorized in writing by an officer
     of Movea. All Rights Reserved.
*/
//-----------------------------------------------------------------------------

#ifndef __ENUMSER_H__
#define __ENUMSER_H__

// STL
#include <string>
#include <vector>

// Poco
#include "Poco/Exception.h"

namespace Poco {
namespace DeviceIO {
namespace Serial {

//-----------------------------------------------------------------------------

class SerialPortEnum
{
public:
	typedef std::vector<std::string> PortList;
	typedef PortList::const_iterator Iterator;
	typedef PortList::const_iterator ConstIterator;
	typedef PortList::const_iterator iterator;
	typedef PortList::const_iterator const_iterator;

	SerialPortEnum();
	~SerialPortEnum();

	Iterator begin() const;
	Iterator end() const;
	int size() const;
	const std::string& operator[] (int index) const;

private:

	PortList _ports;
};

//-----------------------------------------------------------------------------

} } } // namespace Poco::DeviceIO::Serial

#endif //__ENUMSER_H__

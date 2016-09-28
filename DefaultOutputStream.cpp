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

#include "DefaultOutputStream.h"

#include <cassert>

DefaultOutputStream * DefaultOutputStream::_instance = 0;

DefaultOutputStream::DefaultOutputStream()
: _stream(stdout)
{
	assert(!_instance);
	_instance = this;
}

DefaultOutputStream::~DefaultOutputStream()
{
	_instance = 0;
}

DefaultOutputStream & DefaultOutputStream::instance()
{
	assert(_instance);
	return *_instance;
}

void DefaultOutputStream::setStream(FILE* stream)
{
	DefaultOutputStream::ScopedEntry entry;
	entry.setStream(stream);
}

DefaultOutputStream::ScopedEntry::ScopedEntry()
: _lock(DefaultOutputStream::instance()._stream_mutex)
{

}

DefaultOutputStream::ScopedEntry::~ScopedEntry()
{
	fflush(DefaultOutputStream::instance()._stream);
}

FILE* DefaultOutputStream::ScopedEntry::getStream() const
{
	 return DefaultOutputStream::instance()._stream;
}

void DefaultOutputStream::ScopedEntry::setStream(FILE* stream)
{
	assert(stream);
	DefaultOutputStream::instance()._stream = stream;
}

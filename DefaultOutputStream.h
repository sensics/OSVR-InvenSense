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

#ifndef _DEFAULT_OUTPUT_STREAM_H_
#define _DEFAULT_OUTPUT_STREAM_H_

#include "Poco/Mutex.h"

#include <cstdio>

//-----------------------------------------------------------------------------

/// Manage default output stream displaying a lot of information
/// (data event, stats and probes for example)
class DefaultOutputStream
{
public:

	/// Ensure the stream is correctly locked before any operation.
	struct ScopedEntry
	{
		ScopedEntry();
		~ScopedEntry();

		/// Retrieve output stream
		FILE* getStream() const;

		/// Change the default output stream
		void setStream(FILE* stream);

	private:
		Poco::Mutex::ScopedLock _lock;
	};

	DefaultOutputStream();
	~DefaultOutputStream();

	static DefaultOutputStream & instance();

	/// Change the default output stream (thread-safe)
	static void setStream(FILE* stream);

private:
	FILE*                        _stream;
	Poco::Mutex                  _stream_mutex;
	static DefaultOutputStream*  _instance;

};

//-----------------------------------------------------------------------------

#endif // _DEFAULT_OUTPUT_STREAM_H_

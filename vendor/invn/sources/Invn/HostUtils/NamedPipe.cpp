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

#include "NamedPipe.h"

#include <cassert>
#include <stdexcept>

#ifdef _WIN32

#include <Windows.h>
#include <io.h>
#include <Fcntl.h>

class NamedPipeImpl
{
public:
	NamedPipeImpl()
		: _hpipe(INVALID_HANDLE_VALUE)
		, _mode(NamedPipe::MODE_READ)
	{
	}

	~NamedPipeImpl()
	{
		close();
	}

	void open(const std::string & path, NamedPipe::Mode mode, bool create, bool wait)
	{
		assert(_hpipe == INVALID_HANDLE_VALUE);

		const std::string pname = "\\\\.\\pipe\\" + path;
		_mode = mode;

		if(create) {
			const int pipe_access = (mode == NamedPipe::MODE_READ) ?
					PIPE_ACCESS_INBOUND : PIPE_ACCESS_OUTBOUND;

			_hpipe = CreateNamedPipe(pname.c_str(),
					// FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..)
					// to fail if the pipe already exists...
					pipe_access | FILE_FLAG_FIRST_PIPE_INSTANCE,
					PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
					1,
					1024 * 16,
					1024 * 16,
					NMPWAIT_USE_DEFAULT_WAIT,
					NULL);

			if(_hpipe == INVALID_HANDLE_VALUE)
				throw std::runtime_error("Cannot create named pipe '" + pname + "'");
		} else {
			const int pipe_access = (mode == NamedPipe::MODE_READ) ?
					GENERIC_READ : GENERIC_WRITE;

			_hpipe = CreateFile(pname.c_str(),
					pipe_access,
					0,
					NULL,
					OPEN_EXISTING,
					0,
					NULL);

			if(_hpipe == INVALID_HANDLE_VALUE)
				throw std::runtime_error("Cannot open pipe '" + pname + "'");
		}

		if(wait) {
			ConnectNamedPipe(_hpipe, NULL);
		}
	}

	void close(void)
	{
		if(_hpipe != INVALID_HANDLE_VALUE) {
			CloseHandle(_hpipe);
			_hpipe = INVALID_HANDLE_VALUE;
		}
	}

	int read(void * buf, unsigned long nb_byte, int timeout)
	{
		assert(_hpipe != INVALID_HANDLE_VALUE);

		(void)timeout;

		DWORD bytesRead = 0;
		BOOL ok = ReadFile(_hpipe, buf, nb_byte, &bytesRead, NULL);
		DWORD err = 0;
		if(ok || ((err = GetLastError()) == ERROR_BROKEN_PIPE)
				|| (err == ERROR_PIPE_LISTENING) || (err == ERROR_NO_DATA))
			return bytesRead;
		else
			throw std::runtime_error("reading pipe");
	}

	int write(const void * buf, unsigned long nb_byte)
	{
		assert(_hpipe != INVALID_HANDLE_VALUE);

		DWORD bytesWritten = 0;
		BOOL ok;

		do {
			ok = WriteFile(_hpipe, buf, nb_byte, &bytesWritten, NULL);
		} while (!ok && (GetLastError() == ERROR_PIPE_BUSY));

		if(!ok)
			throw std::runtime_error("Error writing to pipe");

		return bytesWritten;
	}

	bool opened(void)
	{
		return (_hpipe != INVALID_HANDLE_VALUE);
	}

	FILE * fdopen(void)
	{
		assert(_hpipe != INVALID_HANDLE_VALUE);

		int flags = 0;
		std::string cmode;

		if(_mode == NamedPipe::MODE_READ) {
			flags |= _O_RDONLY;
			cmode = "rb";
		} else {
			cmode = "wb";
		}

		const int fd = _open_osfhandle((intptr_t)_hpipe, flags);
		if(fd != -1) {
			return _fdopen(fd, cmode.c_str());
		}

		return 0;
	}

private:
	HANDLE          _hpipe;
	NamedPipe::Mode _mode;
};

#else

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include <stdio.h>

class NamedPipeImpl
{
public:
	NamedPipeImpl()
		: _fd(-1)
		, _mode(NamedPipe::MODE_READ)
		, _fhandle(0)
	{
	}

	~NamedPipeImpl()
	{
		close();
	}

	void open(const std::string & path, NamedPipe::Mode mode, bool create, bool wait)
	{
		assert(_fd == -1);

		_mode = mode;

		if(create) {
			if(::mkfifo(path.c_str(), S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP) != 0 && errno != EEXIST)
				throw std::runtime_error("Cannot create named pipe '" + path + "'");
		}

		int flags = (mode == NamedPipe::MODE_READ) ? O_RDONLY : O_WRONLY;

		if(!wait) {
			flags |= O_NONBLOCK;
		}

		_fd = ::open(path.c_str(), flags);

		if(_fd == -1) {
			throw std::runtime_error("Cannot open pipe '" + path + "'");
		}
	}

	void close(void)
	{
		if(_fd != -1) {
			::close(_fd);
			_fd = -1;
		}

		/* No fclosing _fhandle is flagged as a leak by Coverity, but this causes a crash... */

		// if(_fhandle) {
		// 	fclose(_fhandle);
		// 	_fhandle = 0;
		// }
	}

	int read(void * buf, unsigned long nb_byte, int timeout)
	{
		assert(_fd != -1);

		::pollfd ufds = { _fd, POLLIN, 0 };
		int polling;
		do {
			polling = ::poll(&ufds, 1, timeout > 0 ? timeout : -1);
		} while(errno == EINTR);

		if (polling < 0)
			throw std::runtime_error("polling pipe");

		if (polling == 0)
			return 0; // timeout

		if (ufds.revents != POLLIN)
			throw std::runtime_error("polling pipe");

		// reading
		int total = 0;
		int n;
		do {
			if((n = ::read(_fd, buf, nb_byte)) > 0)
				total += n;
		} while (n < 0 && errno == EINTR);

		if (n >= 0)
			return total;
		else
			throw std::runtime_error("reading pipe");
	}

	int write(const void * buf, unsigned long nb_byte)
	{
		assert(_fd != -1);

		int n;

		do {
			n = ::write(_fd, buf, nb_byte);
		} while (n < 0 && errno == EINTR);

		if (n >= 0)
			return n;
		else
			throw std::runtime_error("Error writing to pipe");
	}

	bool opened(void)
	{
		return (_fd != -1);
	}

	FILE * fdopen(void)
	{
		assert(_fd != -1);

		if(_mode == NamedPipe::MODE_READ) {
			_fhandle = ::fdopen(_fd, "r");
		} else {
			_fhandle = ::fdopen(_fd, "w");
		}

		return _fhandle;
	}

private:
	int             _fd;
	NamedPipe::Mode _mode;
	FILE *          _fhandle;
};

#endif

NamedPipe::NamedPipe() :
	_impl(new NamedPipeImpl())
{
}

NamedPipe::NamedPipe(const std::string & path, Mode mode, bool create, bool wait)
	: _impl(new NamedPipeImpl())
{
	open(path, mode, create, wait);
}

NamedPipe::~NamedPipe()
{
	delete _impl;
}

void NamedPipe::open(const std::string & path, Mode mode, bool create, bool wait)
{
	_impl->open(path, mode, create, wait);
}

void NamedPipe::close(void)
{
	_impl->close();
}

int NamedPipe::read(void * buf, unsigned long nb_byte, int timeout)
{
	return _impl->read(buf, nb_byte, timeout);
}

int NamedPipe::write(const void * buf, unsigned long nb_byte)
{
	return _impl->write(buf, nb_byte);
}

bool NamedPipe::opened(void)
{
	return _impl->opened();
}

FILE * NamedPipe::fdopen(void)
{
	if(!_impl->opened())
		return 0;

	return _impl->fdopen();
}

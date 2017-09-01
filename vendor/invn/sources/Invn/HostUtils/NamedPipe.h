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

#ifndef _NAMED_PIPE_H_
#define _NAMED_PIPE_H_

#include "Invn/InvExport.h"

#include <string>

class NamedPipeImpl;

class INV_EXPORT NamedPipe
{
public:
	enum Mode {
		MODE_READ, MODE_WRITE
	};

	NamedPipe(const std::string & path, Mode mode = MODE_READ, bool create = false, bool wait = true);
	NamedPipe();
	~NamedPipe();
	void open(const std::string & path, Mode mode, bool create = false, bool wait = true);
	void close(void);
	int read(void * buf, unsigned long nb_byte, int timeout = 0);
	int write(const void * buf, unsigned long nb_byte);
	bool opened(void);
	FILE * fdopen(void);
private:
	NamedPipeImpl * _impl;
};

#endif /* _NAMED_PIPE_H_ */

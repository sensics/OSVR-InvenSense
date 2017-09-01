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

#include "Os.h"

#include "Invn/InvError.h"

#include "Poco/Thread.h"
#include "Poco/Timestamp.h"
#include "Poco/Mutex.h"
#include "Poco/Semaphore.h"
#include "Poco/Event.h"

/******************************************************************************/

void * inv_os_malloc(size_t size)
{
	return malloc(size);
}

void inv_os_free(void * ptr)
{
	free(ptr);
}

/******************************************************************************/

void inv_os_sleep(uint32_t ms)
{
	Poco::Thread::sleep(ms);
}

void inv_os_sleep_us(uint32_t us)
{
	uint32_t ms = (us/1000);

	if(us%1000) {
		ms += 1;
	}

	Poco::Thread::sleep(ms);
}

uint64_t inv_os_systick_get(void)
{
	Poco::Timestamp timestamp;

	return timestamp.epochMicroseconds();
}

/******************************************************************************/

class inv_os_thread_run: public Poco::Runnable
{
public:
	inv_os_thread_run(void (*routine)(void * arg), void * arg)
		: _routine(routine)
		, _arg(arg)
	{
	}
	~inv_os_thread_run()
	{
	}

	void run()
	{
		_routine(_arg);
	}

private:
	void (*_routine)(void * arg);
	void * _arg;
};

class int_thread_impl : public Poco::Thread
{
public:
	int_thread_impl(void (*routine)(void * arg), void * arg)
		: runnnable(routine, arg)
	{
	}
	~int_thread_impl()
	{
	}

	inv_os_thread_run	runnnable;
};

int inv_os_thread_create(inv_os_thread_t * pth, void (*routine)(void * arg), void * arg)
{
	if(!pth)
		return INV_ERROR;

	int_thread_impl * thread = new int_thread_impl(routine, arg);

	*pth = thread;

	return 0;
}


int inv_os_thread_start(inv_os_thread_t th)
{
	try {
		int_thread_impl * thread = reinterpret_cast<int_thread_impl *>(th);

		thread->start(thread->runnnable);
	} catch(...) {
		return INV_ERROR;
	}

	return 0;
}

int inv_os_thread_join(inv_os_thread_t th, long timeout)
{
	try {
		int_thread_impl * thread = reinterpret_cast<int_thread_impl *>(th);

		if(!th) {
			return INV_ERROR;
		}

		if(timeout == -1) {
			thread->join();
		} else {
			if(!thread->tryJoin(timeout))
				return INV_ERROR_TIMEOUT;
		}
	} catch(...) {
		return INV_ERROR;
	}

	return 0;
}

int inv_os_thread_destroy(inv_os_thread_t th)
{
	int_thread_impl * thread = reinterpret_cast<int_thread_impl *>(th);
	delete thread;
	return 0;
}

/******************************************************************************/

int inv_os_mutex_create(inv_os_mutex_t * pmutex)
{
	if(!pmutex)
		return INV_ERROR;

	Poco::Mutex * mutex = new Poco::Mutex;

	*pmutex = mutex;

	return 0;
}

int inv_os_mutex_destroy(inv_os_mutex_t mutex)
{
	Poco::Mutex * mu = reinterpret_cast<Poco::Mutex *>(mutex);

	delete mu;

	return 0;
}

int inv_os_mutex_lock(inv_os_mutex_t mutex, long timeout)
{
	try {
		Poco::Mutex * mu = reinterpret_cast<Poco::Mutex *>(mutex);

		if(timeout == 0) {
			if(!mu->tryLock())
				return INV_ERROR_TIMEOUT;
		} else if(timeout == -1) {
			mu->lock();
		} else {
			if(!mu->tryLock(timeout))
				return INV_ERROR_TIMEOUT;
		}
	} catch(...) {
		return INV_ERROR;
	}

	return 0;
}

int inv_os_mutex_unlock(inv_os_mutex_t mutex)
{
	try {
		Poco::Mutex * mu = reinterpret_cast<Poco::Mutex *>(mutex);

		mu->unlock();
	} catch(...) {
		return INV_ERROR;
	}

	return 0;
}

/******************************************************************************/

int inv_os_sem_create(inv_os_sem_t * psem, int value, int max)
{
	if(!psem)
		return INV_ERROR;

	Poco::Semaphore * semaphore = new Poco::Semaphore(value, max);

	*psem = semaphore;

	return 0;
}

int inv_os_sem_destroy(inv_os_sem_t sem)
{
	Poco::Semaphore * semaphore = reinterpret_cast<Poco::Semaphore *>(sem);

	delete semaphore;

	return 0;
}

int inv_os_sem_post(inv_os_sem_t sem)
{
	try {
		Poco::Semaphore * semaphore = reinterpret_cast<Poco::Semaphore *>(sem);

		semaphore->set();
	} catch(...) {
		return INV_ERROR;
	}

	return 0;
}

int inv_os_sem_wait(inv_os_sem_t sem, long timeout)
{
	try {
		Poco::Semaphore * semaphore = reinterpret_cast<Poco::Semaphore *>(sem);

		if(timeout == -1) {
			semaphore->wait();
		} else {
			if(!semaphore->tryWait(timeout))
				return INV_ERROR_TIMEOUT;
		}
	} catch(...) {
		return INV_ERROR;
	}

	return 0;
}

/******************************************************************************/

int inv_os_event_create(inv_os_event_t * pevent)
{
	if(!pevent)
		return INV_ERROR;
	Poco::Event * event = new Poco::Event();

	*pevent = event;

	return 0;
}

int inv_os_event_destroy(inv_os_event_t event)
{
	Poco::Event * e = reinterpret_cast<Poco::Event *>(event);

	delete e;

	return 0;
}

int inv_os_event_reset(inv_os_event_t event)
{
	try {
		Poco::Event * e = reinterpret_cast<Poco::Event *>(event);

		e->reset();
	} catch(...) {
		return INV_ERROR;
	}

	return 0;
}

int inv_os_event_set(inv_os_event_t event)
{
	try {
		Poco::Event * e = reinterpret_cast<Poco::Event *>(event);

		e->set();
	} catch(...) {
		return INV_ERROR;
	}

	return 0;
}

int inv_os_event_wait(inv_os_event_t event, long timeout)
{
	try {
		Poco::Event * e = reinterpret_cast<Poco::Event *>(event);

		if(timeout == -1) {
			e->wait();
		} else {
			if(!e->tryWait(timeout))
				return INV_ERROR_TIMEOUT;
		}
	} catch(...) {
		return INV_ERROR;
	}

	return 0;
}

/******************************************************************************/

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

/** @defgroup Os Os
 *	@brief    Wrapper around basic operating system primitives to abstract unerlying OS
 *  @ingroup  Utils
 *	@{
 */

#ifndef _INV_OS_H_
#define _INV_OS_H_

#include "Invn/InvExport.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

#include "Invn/InvError.h"

/**	@brief      Allocate  size bytes and returns a pointer to the allocated memory
 *  @param[in]  Number of bytes to allocate
 *  @return     Pointer to allocated memory or NULL if none
 */
void INV_EXPORT * inv_os_malloc(size_t size);

/**	@brief      Release previously allocated memory
 *  @param[in]  Pointer to memory allocated by inv_malloc
 *  @return     none
 */
void INV_EXPORT inv_os_free(void * ptr);

/**	@brief      Make calling thread sleep for a period of time
 *  @param[in]  ms 	time in millisecond
 *  @return     none
 */
void INV_EXPORT inv_os_sleep(uint32_t ms);

/**	@brief      Make calling thread sleep for a period of time
 *  @param[in]  us 	time in microsecond
 *  @warning    resolution is ms
 *  @return     none
 */
void INV_EXPORT inv_os_sleep_us(uint32_t us);

/**	@brief      Alias of inv_os_sleep()
 */
static inline void inv_os_msleep(uint32_t ms)
{
	inv_os_sleep(ms);
}

/**	@brief      Return a monotonic timestamp in microsecond
 */
uint64_t INV_EXPORT inv_os_systick_get(void);

/**	@brief      Thread handle definition
 */
typedef void * inv_os_thread_t;

/**	@brief      Create a thread
 *  @param[out] pth     handle to created thread
 *  @param[in]  routine thread main function
 *  @param[in]  arg     context passed to the thread main
 *  @return     0 on success, negative value on error
 */
int INV_EXPORT inv_os_thread_create(inv_os_thread_t * pth, void (*routine)(void * arg), void * arg);

/**
 * @brief      Start a thread
 * @param th   thread handle
 * @return     0 on success, negative value on error
 */
int INV_EXPORT inv_os_thread_start(inv_os_thread_t th);

/**	@brief      Wait for a thread to return
 *  @param[in]  th     	thread handle
 *  @param[in]  timeout wait for at most specified timeout time in ms or -1 to wait forever
 *  @return     0 on success, negative value on error
 */
int INV_EXPORT inv_os_thread_join(inv_os_thread_t th, long timeout);

/** @brief      Destroy a thread
 *  @param th   thread handle
 *  @return     0 on success, negative value on error
 */
int INV_EXPORT inv_os_thread_destroy(inv_os_thread_t th);

/**	@brief      Mutex handle definition
 */
typedef void * inv_os_mutex_t;

/**	@brief      Create and initialized a mutex
 *  @param[out] pmutex   handle to created mutext
 *  @return     0 on sucess, negative value on error
 */
int INV_EXPORT inv_os_mutex_create(inv_os_mutex_t * pmutex);

/**	@brief      Destroy a previously created mutex
 *  @param[in]  mutex  mutex handle
 *  @return     0 on sucess, negative value on error
 */
int INV_EXPORT inv_os_mutex_destroy(inv_os_mutex_t mutex);

/**	@brief      Lock a mutex. Will block calling thread if mutex is already locked
 *  @param[in]  mutex    mutex handle
 *  @param[in]  timeout  block for at most specified timeout time in ms or -1 to block forever
 *  @return     0 on sucess, TIMEOUT error if mutex could not be locked before timeout
 */
int INV_EXPORT inv_os_mutex_lock(inv_os_mutex_t mutex, long timeout);

/**	@brief      Unlock a mutex
 *  @param[in]  mutex    mutex handle
 *  @return     0 on sucess, negative value on error
 */
int INV_EXPORT inv_os_mutex_unlock(inv_os_mutex_t mutex);

/**	@brief      Semaphore handle definition
 */
typedef void * inv_os_sem_t;

/**	@brief      Create and initialized a semaphore
 *  @param[out] psem   handle to created semaphore
 *  @param[in]  value  initial semaphore value
 *  @param[in]  mas    maximum semaphore value
 *  @return     0 on sucess, negative value on error
 */
int INV_EXPORT inv_os_sem_create(inv_os_sem_t * psem, int value, int max);

/**	@brief      Destroy a previously created semaphore
 *  @param[in]  sem  semaphore handle
 *  @return     0 on sucess, negative value on error
 */
int INV_EXPORT inv_os_sem_destroy(inv_os_sem_t sem);

/**	@brief      Increment the semaphore's value by one and thus signals the semaphore
 *  @param[in]  sem    semaphore handle
 *  @return     0 on sucess, negative value on error
 */
int INV_EXPORT inv_os_sem_post(inv_os_sem_t sem);

/**	@brief      Wait for the semaphore to become signalled
 *  @param[in]  sem    semaphore handle
 *  @param[in]  timeout  block for at most specified timeout time in ms or -1 to wait forever
 *  @return     0 on sucess, TIMEOUT error if mutex could not be locked before timeout
 */
int INV_EXPORT inv_os_sem_wait(inv_os_sem_t sem, long timeout);

/**	@brief      Event handle definition
 */
typedef void * inv_os_event_t;

/**	@brief      Create and initialized an event
 *              Events act as binary semaphore
 *  @param[out] pevent handle to created event object
 *  @return     0 on sucess, negative value on error
 */
int INV_EXPORT inv_os_event_create(inv_os_event_t * pevent);

/**	@brief      Destroy a previously created event
 *  @param[in]  event  event handle
 *  @return     0 on sucess, negative value on error
 */
int INV_EXPORT inv_os_event_destroy(inv_os_event_t event);

/**	@brief      Reset the event
 *  @param[in]  event    event handle
 *  @return     0 on sucess, negative value on error
 */
int INV_EXPORT inv_os_event_reset(inv_os_event_t event);

/**	@brief      Signal the event to unblock thread waiting on the event
 *  @param[in]  event    event handle
 *  @return     0 on sucess, negative value on error
 */
int INV_EXPORT inv_os_event_set(inv_os_event_t event);

/**	@brief      Wait for the eventaphore to become signalled
 *  @param[in]  event    event handle
 *  @param[in]  timeout  block for at most specified timeout time in ms or -1 to wait forever
 *  @return     0 on sucess, TIMEOUT error if mutex could not be locked before timeout
 */
int INV_EXPORT inv_os_event_wait(inv_os_event_t event, long timeout);

#ifdef __cplusplus
}
#endif

#endif /* _INV_OS_UTILS_H_ */

/** @} */

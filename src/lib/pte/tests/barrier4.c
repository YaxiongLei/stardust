/* Copyright (C) 2017, Ward Jaradat and Jonathan Lewis
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 * barrier4.c
 *
 *
 * --------------------------------------------------------------------------
 *
 *      Pthreads-embedded (PTE) - POSIX Threads Library for embedded systems
 *      Copyright(C) 2008 Jason Schmidlapp
 *
 *      Contact Email: jschmidlapp@users.sourceforge.net
 *
 *
 *      Based upon Pthreads-win32 - POSIX Threads Library for Win32
 *      Copyright(C) 1998 John E. Bossom
 *      Copyright(C) 1999,2005 Pthreads-win32 contributors
 *
 *      Contact Email: rpj@callisto.canberra.edu.au
 *
 *      The original list of contributors to the Pthreads-win32 project
 *      is contained in the file CONTRIBUTORS.ptw32 included with the
 *      source code distribution. The list can also be seen at the
 *      following World Wide Web location:
 *      http://sources.redhat.com/pthreads-win32/contributors.html
 *
 *      This library is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Lesser General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 *
 *      This library is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Lesser General Public License for more details.
 *
 *      You should have received a copy of the GNU Lesser General Public
 *      License along with this library in the file COPYING.LIB;
 *      if not, write to the Free Software Foundation, Inc.,
 *      59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 *
 * --------------------------------------------------------------------------
 *
 * Declare a single barrier object, multiple wait on it,
 * and then destroy it.
 *
 */

#include <os/config.h>

#ifdef ENABLE_PTE_TESTS

#include <pte/test.h>

enum
{
  NUMTHREADS = 16
};

static pthread_barrier_t barrier = NULL;
static pthread_mutex_t mx = PTHREAD_MUTEX_INITIALIZER;
static int serialThreadCount = 0;
static int otherThreadCount = 0;

static void *
func(void * arg)
{
	int result = pthread_barrier_wait(&barrier);

	assert(pthread_mutex_lock(&mx) == 0);

	if (result == PTHREAD_BARRIER_SERIAL_THREAD)
	{
		serialThreadCount++;
	}
	else if (0 == result)
	{
		otherThreadCount++;
	}
  else
    {
    	// Unlocking the mutex here before returning, otherwise the kernel will crash
		assert(pthread_mutex_unlock(&mx) == 0);
		return NULL;
    }
	assert(pthread_mutex_unlock(&mx) == 0);
	return NULL;
}

int pthread_test_barrier4()
{
	int i, j;
	pthread_t t[NUMTHREADS + 1];

	//  pthread_barrier_t barrier = NULL;
	//  serialThreadCount = 0;
	//  otherThreadCount = 0;

	mx = PTHREAD_MUTEX_INITIALIZER;

	for (j = 1; j <= NUMTHREADS; j++)
	{

		serialThreadCount = 0;

		assert(pthread_barrier_init(&barrier, NULL, j) == 0);

		for (i = 1; i <= j; i++)
		{
			assert(pthread_create(&t[i], NULL, func, NULL) == 0);
		}

		for (i = 1; i <= j; i++)
		{
			assert(pthread_join(t[i], NULL) == 0);
		}

		assert(serialThreadCount == 1);

		assert(pthread_barrier_destroy(&barrier) == 0);

	}

	assert(pthread_mutex_destroy(&mx) == 0);

	return 0;
}

#endif

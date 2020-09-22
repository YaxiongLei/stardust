/* Copyright (C) 2017, Ward Jaradat
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
 * pthread_rwlock_tryrdlock.c
 *
 * Description:
 * This translation unit implements read/write lock primitives.
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
 */

#include <os/config.h>

#ifdef ENABLE_PTE

#include <errno.h>
#include <pte/pthread.h>
#include <pte/implement.h>
#include <pte/pte_generic_osal.h>

int
pthread_rwlock_tryrdlock (pthread_rwlock_t * rwlock)
{
	int result;
	pthread_rwlock_t rwl;

	if (rwlock == NULL || *rwlock == NULL)
	{
		return EINVAL;
	}

	/*
	 * We do a quick check to see if we need to do more work
	 * to initialise a static rwlock. We check
	 * again inside the guarded section of pte_rwlock_check_need_init()
	 * to avoid race conditions.
	 */
	if (*rwlock == PTHREAD_RWLOCK_INITIALIZER)
	{
		result = pte_rwlock_check_need_init (rwlock);

		if (result != 0 && result != EBUSY)
		{
			return result;
		}
	}

	rwl = *rwlock;

	if (rwl->nMagic != PTE_RWLOCK_MAGIC)
	{
		return EINVAL;
	}

	if ((result = pthread_mutex_trylock (&(rwl->mtxExclusiveAccess))) != 0)
	{
		return result;
	}

	if (++rwl->nSharedAccessCount == INT_MAX)
	{
		if ((result =
				pthread_mutex_lock (&(rwl->mtxSharedAccessCompleted))) != 0)
		{
			(void) pthread_mutex_unlock (&(rwl->mtxExclusiveAccess));
			return result;
		}

		rwl->nSharedAccessCount -= rwl->nCompletedSharedAccessCount;
		rwl->nCompletedSharedAccessCount = 0;

		if ((result =
				pthread_mutex_unlock (&(rwl->mtxSharedAccessCompleted))) != 0)
		{
			(void) pthread_mutex_unlock (&(rwl->mtxExclusiveAccess));
			return result;
		}
	}

	return (pthread_mutex_unlock (&rwl->mtxExclusiveAccess));
}

#endif
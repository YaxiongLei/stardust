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
 * pthread_barrier_init.c
 *
 * Description:
 * This translation unit implements barrier primitives.
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

#include <stdlib.h>
#include <pte/pthread.h>
#include <pte/implement.h>
#include <errno.h>
#include <stdlib.h>
#include <pte/pte_generic_osal.h>

int
pthread_barrier_init (pthread_barrier_t * barrier,
		const pthread_barrierattr_t * attr, unsigned int count)
{
	pthread_barrier_t b;

	if (barrier == NULL || count == 0)
	{
		return EINVAL;
	}

	if (NULL != (b = (pthread_barrier_t) calloc (1, sizeof (*b))))
	{
		b->pshared = (attr != NULL && *attr != NULL
				? (*attr)->pshared : PTHREAD_PROCESS_PRIVATE);

		b->nCurrentBarrierHeight = b->nInitialBarrierHeight = count;
		b->iStep = 0;

		/*
		 * Two semaphores are used in the same way as two stepping
		 * stones might be used in crossing a stream. Once all
		 * threads are safely on one stone, the other stone can
		 * be moved ahead, and the threads can start moving to it.
		 * If some threads decide to eat their lunch before moving
		 * then the other threads have to wait.
		 */
		if (0 == sem_init (&(b->semBarrierBreeched[0]), b->pshared, 0))
		{
			if (0 == sem_init (&(b->semBarrierBreeched[1]), b->pshared, 0))
			{
				*barrier = b;
				return 0;
			}
			(void) sem_destroy (&(b->semBarrierBreeched[0]));
		}
		(void) free (b);
	}

	return ENOMEM;
}

#endif
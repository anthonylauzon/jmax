/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "ftsconfig.h"

#include <fts/fts.h>

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include <sys/types.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef WIN32
/* included for the select() function */
#include <windows.h>
#endif

typedef struct sched_callback
{
  fts_object_t *object;
  int flags;
  int fd;
  fts_method_t ready_mth;
  fts_method_t error_mth;
  struct sched_callback *next;
} sched_callback_t;

typedef struct fts_sched
{
  sched_callback_t *callback_head; 
  enum {sched_ready, sched_halted} status;
  fts_clock_t clock; /* clock of logical time */
} fts_sched_t;


/* the global scheduler */
static fts_sched_t main_sched;

static fts_sched_t *fts_sched_get_current(void)
{
  return &main_sched;
}

double fts_get_time(void)
{
  return main_sched.clock.time;
}

/*****************************************************************************
 *
 *  select and file descrpitors
 *
 */

int fts_sched_add( fts_object_t *obj, int flags, ...)
{
  va_list ap;
  fts_sched_t *sched = fts_sched_get_current();
  sched_callback_t *callback, **p;
  int fd = -1;
  fts_method_t mth;

  va_start( ap, flags);
  if (flags == FTS_SCHED_READ || flags == FTS_SCHED_WRITE)
    fd = va_arg( ap, int);

  /* Add the new entry at end of list, so that the functions will be called in
     order of add */
  p = &sched->callback_head;

  while (*p)
    {
      if ( (*p)->object == obj)
	return -1;

      p = &(*p)->next;
    }

  callback = (sched_callback_t *)fts_malloc( sizeof( sched_callback_t));
  callback->object = obj;
  callback->flags = flags;
  callback->fd = fd;

  mth = fts_class_get_method( fts_object_get_class(obj), fts_SystemInlet, fts_s_sched_ready);
  if ( !mth)
    return -1;

  callback->ready_mth = mth;
  callback->error_mth = fts_class_get_method( fts_object_get_class(obj), fts_SystemInlet, fts_s_sched_error);

  callback->next = 0;

  *p = callback;

  va_end( ap);

  return 0;
}

int fts_sched_remove( fts_object_t *obj)
{
  sched_callback_t **p;
  fts_sched_t *sched = fts_sched_get_current();

  p = &sched->callback_head;

  while (*p)
    {
      if ( (*p)->object == obj)
	{
	  sched_callback_t *to_remove;

	  to_remove = *p;
	  *p = (*p)->next;

	  fts_free( to_remove);
	}
      else
	p = &(*p)->next;
    }

  return -1;
}

static int compute_fds( fts_sched_t *sched, fd_set *readfds, fd_set *writefds, fd_set *exceptfds)
{
  sched_callback_t *callback;
  int n_fd = 0;

  FD_ZERO( readfds);
  FD_ZERO( writefds);
  FD_ZERO( exceptfds);

  n_fd = 0;
  for ( callback = sched->callback_head; callback; callback = callback->next)
    {
      if (callback->flags == FTS_SCHED_ALWAYS)
	continue;

      if ( callback->fd > n_fd)
	n_fd = callback->fd;

      if (callback->flags == FTS_SCHED_READ)
	FD_SET( callback->fd, readfds);
      else
	FD_SET( callback->fd, writefds);

      FD_SET( callback->fd, exceptfds);
    }

  return n_fd;
}

static void run_select( fts_sched_t *sched, int n_fd, fd_set *readfds, fd_set *writefds, fd_set *exceptfds)
{
  struct timeval tv;
  sched_callback_t *callback;
  int r;

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  r = select( n_fd+1, readfds, writefds, exceptfds, &tv);

  if (r < 0)
    {
      fprintf( stderr, "[FTS] select error (%s)\n", strerror( errno));
      return;
    }
  else if ( r == 0)
    return;

  for ( callback = sched->callback_head; callback; callback = callback->next)
    {
      int fd = callback->fd;
      fts_atom_t a;

      fts_set_int( &a, fd);

      if ( callback->error_mth && FD_ISSET( fd, exceptfds))
	(*callback->error_mth)( callback->object, fts_SystemInlet, fts_s_sched_error, 1, &a);
      else
	{
	  if ( (callback->flags == FTS_SCHED_READ && FD_ISSET( fd, readfds))
	       || (callback->flags == FTS_SCHED_WRITE && FD_ISSET( fd, writefds)) )
	    (*callback->ready_mth)( callback->object, fts_SystemInlet, fts_s_sched_ready, 1, &a);
	}
    }
}

static void run_always( fts_sched_t *sched)
{
  sched_callback_t *callback;

  for ( callback = sched->callback_head; callback; callback = callback->next)
    {
      if ( callback->flags == FTS_SCHED_ALWAYS)
	(*callback->ready_mth)( callback->object, fts_SystemInlet, fts_s_sched_ready, 0, 0);
    }
}

static void fts_sched_do_select(fts_sched_t *sched)
{
  fd_set readfds, writefds, exceptfds;
  int r, n_fd;

  n_fd = compute_fds( sched, &readfds, &writefds, &exceptfds);

  if (n_fd != 0)
    run_select( sched, n_fd, &readfds, &writefds, &exceptfds);

  run_always( sched);
}

/*****************************************************************************
 *
 * Clocks
 *
 */

fts_clock_t *fts_sched_get_clock( void)
{
  return &fts_sched_get_current()->clock;
}

void fts_sched_advance_clock( double time)
{
  fts_clock_advance( fts_sched_get_clock(), time);
}

void fts_sched_set_time( double time)
{
  fts_clock_set_time( fts_sched_get_clock(), time);
}

void
fts_sched_init(fts_sched_t *sched)
{
  sched->callback_head = 0;
  fts_clock_init(&sched->clock);
  sched->status = sched_ready;
}

/* run the scheduler in a loop (note that the clock time is set inside fts_dsp_run_tick() */
void fts_sched_run( void)
{
  fts_sched_t *sched = fts_sched_get_current();

  while(sched->status != sched_halted)
    {
      /* poll file descriptors and run functions inserted to the scheduler */
      fts_sched_do_select(sched);

      /* run dsp subsustem */
      fts_dsp_run_tick();
    }
}

void fts_sched_halt( void)
{
  fts_sched_t *sched = fts_sched_get_current();

  sched->status = sched_halted;
}

/*****************************************************************************
 *
 *  Initialization of the scheduler module
 *
 */

void fts_kernel_sched_init(void)
{
  fts_sched_init(&main_sched);
}


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
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fts/sys.h>
#include <fts/lang.h>
#include <fts/runtime/time/time.h>
#include <fts/runtime/sched/sched.h>

#define FD_NO_SELECT -1

fts_sched_t main_sched;

fts_sched_t *
fts_sched_get_current(void)
{
  return &main_sched;
}

double 
fts_get_time(void)
{
  return main_sched.clock.time;
}

/*****************************************************************************
 *
 *  select and file descrpitors
 *
 */

static void fts_sched_add_fd_realize( fts_sched_t *sched, int fd, int read, fts_method_t method, fts_object_t *object)
{
  fd_callback_t *callback;
  fd_callback_t **p;

  callback = (fd_callback_t *)fts_malloc( sizeof( fd_callback_t));
  callback->fd = fd;
  callback->read = read;
  callback->method = method;
  callback->object = object;
  callback->next = 0;

  /* Add the new entry at end of list, so that the functions will be called in
     order of add */
  p = &sched->fd_callback_head;

  while (*p)
    {
      p = &(*p)->next;
    }

  *p = callback;
}

void fts_sched_add_fd( fts_sched_t *sched, int fd, int read, fts_method_t method, fts_object_t *object)
{
  if (fd < 0)
    return;

  fts_sched_add_fd_realize( sched, fd, read, method, object);
}

void fts_sched_add( fts_sched_t *sched, fts_method_t method, fts_object_t *object)
{
  fts_sched_add_fd_realize( sched, FD_NO_SELECT, 1, method, object);
}

void fts_sched_remove_fd( fts_sched_t *sched, int fd)
{
  fd_callback_t **p;

  p = &sched->fd_callback_head;

  while (*p)
    {
      if ( (*p)->fd == fd)
	{
	  fd_callback_t *to_remove;

	  to_remove = *p;
	  *p = (*p)->next;

	  fts_free( to_remove);
	}
      else
	p = &(*p)->next;
    }
}

void fts_sched_remove( fts_sched_t *sched, fts_object_t *object)
{
  fd_callback_t **p;

  p = &sched->fd_callback_head;

  while (*p)
    {
      if ( (*p)->fd == FD_NO_SELECT && (*p)->object == object)
	{
	  fd_callback_t *to_remove;

	  to_remove = *p;
	  *p = (*p)->next;

	  fts_free( to_remove);
	}
      else
	p = &(*p)->next;
    }
}

/*
  Function: fts_sched_do_select
  Description:
    Do a single select() on all the files in the file descriptor set of the sched.
  Arguments:
    sched: the sched owning the file descriptor set
  Returns: nothing.
*/
static void 
fts_sched_do_select(fts_sched_t *sched)
{
  fd_set rfds, wfds;
  struct timeval tv;
  fd_callback_t *callback;
  int r, n_fd;

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  FD_ZERO( &rfds);
  FD_ZERO( &wfds);

  n_fd = 0;
  for ( callback = sched->fd_callback_head; callback; callback = callback->next)
      {
	if (callback->fd == FD_NO_SELECT)
	  continue;

	if ( callback->fd > n_fd)
	  n_fd = callback->fd;

	if(callback->fd >=0)
	  {
	    if (callback->read)
	      FD_SET( callback->fd, &rfds);
	    else
	      FD_SET( callback->fd, &wfds);
	  }
      }

  r = select( n_fd+1, &rfds, &wfds, NULL, &tv);

  for ( callback = sched->fd_callback_head; callback; callback = callback->next)
    {
      int fd;

      fd = callback->fd;
      if ( fd == FD_NO_SELECT)
	{
	  /* Special entry that is called without select() */
	  (*(callback->method))( callback->object, -1, 0, 0, 0);
	}
      else if ( FD_ISSET( fd, &rfds) || FD_ISSET( fd, &wfds))
	{
	  fts_atom_t a;

	  fts_set_int( &a, fd);
	  (*(callback->method))( callback->object, -1, 0, 1, &a);
	}
    }
}

/*****************************************************************************
 *
 *  scheduler
 *
 */

/* origial scheduler fuctions
void fts_sched_run_one_tick( void)
{
  fts_alarm_poll();
  fts_client_poll();
  fts_sched_do_select( fts_sched_get_current());
  fts_dsp_chain_poll();

  fts_sched_ticks += 1.0;
  fts_sched_msecs = fts_sched_ticks * fts_sched_tick_duration;
}

void 
fts_sched_run( void)
{
  fts_running_status = running;

  while(fts_running_status == running)
    {
      fts_alarm_poll();
      fts_client_poll();
      fts_sched_do_select(fts_sched_get_current());
      fts_dsp_chain_poll();
  
      fts_sched_ticks += 1.0;
      fts_sched_msecs = fts_sched_ticks * fts_sched_tick_duration;
    }
}
*/

void
fts_sched_init(fts_sched_t *sched)
{
  sched->fd_callback_head = 0;
  fts_clock_init(&sched->clock);
  sched->status = sched_ready;
}

/* run the scheduler in a loop (note that the clock time is set inside fts_dsp_run_tick() */
void 
fts_sched_run(fts_sched_t *sched)
{
  while(sched->status != sched_halted)
    {
      /* get client events */
      fts_client_poll(); /* still buarghhh! */

      /* poll file descriptors and run functions inserted to the scheduler */
      fts_sched_do_select(sched);

      /* run dsp subsustem */
      fts_dsp_run_tick();
    }
}

void 
fts_sched_halt(fts_sched_t *sched)
{
  sched->status = sched_halted;
}

/*****************************************************************************
 *
 *  scheduler module
 *
 */

static void fts_sched_module_init(void)
{
  fts_sched_init(&main_sched);
}

fts_module_t fts_sched_module = { "sched", "the sched module", fts_sched_module_init, 0, 0};


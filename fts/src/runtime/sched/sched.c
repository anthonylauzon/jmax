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
#include <fts/runtime.h>

double fts_sched_msecs = 0.0;
double fts_sched_ticks = 0.0;
int fts_sched_tick_size = FTS_DEF_TICK_SIZE;
double fts_sched_tick_duration = ((double)(FTS_DEF_TICK_SIZE * 1000) / 44100.0);

#define FD_NO_SELECT -1


/* the pause length; it say how often (in scheduler ticks) 
   we need to give the processor control back to the OS,
   in order to don't overload the machine. */

static int fts_pause_period = 12;


/*****************************************************************************
 *
 *  select and file descrpitors
 *
 */

typedef struct _fd_callback_t 
{
  int fd;
  int read; /* if 1, the file descriptor will be added to the read fdset, if not to the write fd set */
  fts_method_t method;
  fts_object_t *object;
  struct _fd_callback_t *next;
} fd_callback_t;

struct _fts_sched 
{
  fd_callback_t *fd_callback_head;
};

static struct _fts_sched _mschd;
static fts_sched_t *main_sched = 0;

fts_sched_t *fts_sched_get_current( void)
{
  if (!main_sched)
    {
      _mschd.fd_callback_head = 0;
      main_sched = &_mschd;
    }

  return main_sched;
}

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
static void fts_sched_do_select( fts_sched_t *sched)
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
 *  the scheduler loop
 *
 */

static enum {running, halted} fts_running_status;

void fts_halt(void)
{
  fts_running_status = halted;
}

void fts_sched_run()
{
  int tick_counter = 0;

  fts_running_status = running;

  while(fts_running_status == running)
    {
      fts_midi_poll();
      fts_alarm_poll();
      fts_client_poll();
      fts_sched_do_select( fts_sched_get_current());
      fts_dsp_chain_poll();

      fts_sched_ticks += 1.0;
      fts_sched_msecs = fts_sched_ticks * fts_sched_tick_duration;

      tick_counter++;

      if (tick_counter > fts_pause_period)
	{
	  fts_pause(); /* call the OS dependent pause function */
	  tick_counter = 0;
	}
    }
}


/*****************************************************************************
 *
 *  tick and pause
 *
 */

static void fts_sched_set_sampling_rate(void *listener, fts_symbol_t name, const fts_atom_t *value)
{
  if (fts_is_number(value))
    {
      float sr = fts_get_number_float(value);
      fts_sched_tick_duration = fts_sched_tick_size * 1000. / sr;
    }
}


/* Pause handling  */
/* (fd)
   Note: the pause mechanism does not work... (see non-portable/linux.../syssched.c) 
*/
void fts_sched_set_pause(int p)
{
  fts_pause_period = p;
}

/*****************************************************************************
 *
 *  scheduler module
 *
 */

static void fts_sched_init(void)
{
  /* define the scheduler clocks */
  fts_clock_define_protected(fts_new_symbol("msec"), &fts_sched_msecs);
  fts_clock_define_protected(fts_new_symbol("tick"), &fts_sched_ticks);

  /* Install the "msec" clock as default */
  fts_set_default_clock(fts_new_symbol("msec"));

  /* Install the listener for the sampling rate parameter;
   * the actual listener is a null pointer, no state to pass to the handler
   * and we will not deinstall the listener.
   */

  fts_param_add_listener(fts_s_sampling_rate, 0, fts_sched_set_sampling_rate);
}

fts_module_t fts_sched_module = { "sched", "the sched module", fts_sched_init, 0, 0};


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
#include "sys.h"
#include "lang.h"
#include "runtime/sched.h"

/* The two clock variables */

static double schedtime_ms_clock = 0.0;
static double schedtime_tick_clock = 0.0;

/* The scheduler tick length in millisecond;
   set by a sampling rate parameter listener
   */

static float tick_length = ((MAXVS * 1000) / 44100.0); /* set the default scheduler tick */


/* the pause length; it say how often (in scheduler ticks) 
   we need to give the processor control back to the OS,
   in order to don't overload the machine.

   Default to 3 hundreds of ticks (around 400 msec, with 44.1Khz/64 samples per ticks)
   */

static int fts_pause_period = 12;

/* forward declarations */

static void fts_sched_set_sampling_rate(void *listener, fts_symbol_t name, const fts_atom_t *value);

/*****************************************************************************/
/*                                                                           */
/*          Scheduler initialization                                         */
/*                                                                           */
/*****************************************************************************/

void fts_sched_setup(void)
{
  /* Install the timer */

  fts_clock_define_protected(fts_new_symbol("msec"), &schedtime_ms_clock);
  fts_clock_define_protected(fts_new_symbol("tick"), &schedtime_tick_clock);

  /* Install the "ms" timer as default */

  fts_set_default_clock(fts_new_symbol("msec"));

  /* Install the listener for the sampling rate parameter;
   * the actual listener is a null pointer, no state to pass to the handler
   * and we will not deinstall the listener.
   */

  fts_param_add_listener(fts_s_sampling_rate, 0, fts_sched_set_sampling_rate);
}


typedef struct _fd_entry_t {
  int fd;
  fts_thread_fd_fun_t fun;
  void *data;
} fts_fd_entry_t;

typedef struct _fts_fd_set_t {
  fts_fd_entry_t *entries;
  int n_fd;
} fts_fd_set_t;

struct _fts_thread {
  fts_fd_set_t fd_set;
};

static struct _fts_thread _mthrd;
static fts_thread_t main_thread = 0;


static void fts_fd_set_init( fts_fd_set_t *set)
{
  set->entries = NULL;
  set->n_fd = 0;
}

static void fts_fd_set_add( fts_fd_set_t *set, int fd, fts_thread_fd_fun_t fun, void *data)
{
  fts_fd_entry_t *e;

  if ( set->n_fd < fd + 1)
    {
      int i, new_size;

      new_size = fd + 1;
      set->entries = (fts_fd_entry_t *)fts_realloc( set->entries, new_size * sizeof( fts_fd_entry_t));
      for ( i = set->n_fd; i < new_size; i++)
	{
	  set->entries[i].fd = -1;
	  set->entries[i].fun = (fts_thread_fd_fun_t)0;
	  set->entries[i].data = 0;
	}

      set->n_fd = new_size;
    }

  e = &(set->entries[fd]);
  e->fd = fd;
  e->fun = fun;
  e->data = data;
}

static void fts_fd_set_remove( fts_fd_set_t *set, int fd)
{
  set->entries[fd].fd = -1;
  set->entries[fd].fun = (fts_thread_fd_fun_t)0;
  set->entries[fd].data = 0;
}

static void fts_fd_set_do_select( fts_fd_set_t *set)
{
  fd_set fds;
  struct timeval tv;
  int r, i;

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  FD_ZERO( &fds);

  for ( i = 0; i < set->n_fd; i++)
    if ( set->entries[i].fd >= 0)
	FD_SET( set->entries[i].fd, &fds);

  r = select( set->n_fd, &fds, NULL, NULL, &tv);

  for ( i = 0; i < set->n_fd; i++)
    if ( set->entries[i].fd >= 0)
	if ( FD_ISSET( set->entries[i].fd, &fds))
	    (*(set->entries[i].fun))( set->entries[i].fd, set->entries[i].data);
}

fts_thread_t fts_thread_get_current( void)
{
  if (!main_thread)
    {
      fts_fd_set_init( &(_mthrd.fd_set));
      main_thread = &_mthrd;
    }

  return main_thread;
}

fts_thread_t fts_thread_new( void (*start)( void *))
{
  return NULL;
}

void fts_thread_start( fts_thread_t thread, void *arg)
{
}

void fts_thread_add_fd( fts_thread_t thread, int fd, fts_thread_fd_fun_t fun, void *data)
{
  fts_fd_set_add( &(thread->fd_set), fd, fun, data);
}

void fts_thread_remove_fd( fts_thread_t thread, int fd)
{
  fts_fd_set_remove( &(thread->fd_set), fd);
}

void fts_thread_do_select( fts_thread_t thread)
{
  fts_fd_set_do_select( &(thread->fd_set));
}



/*****************************************************************************/
/*                                                                           */
/*          Run Time Scheduler                                               */
/*                                                                           */
/*****************************************************************************/

static enum {running, halted} fts_running_status;

void fts_halt(void)
{
  fts_running_status = halted;
}

void fts_sched_run(void)
{
  int tick_counter = 0;

  fts_running_status = running;

  while (fts_running_status == running)
    {
      midi_poll();
      fts_alarm_poll();
      client_poll();
      fd_set_poll();
      dsp_chain_poll();

      schedtime_tick_clock += 1.0;
      schedtime_ms_clock =  schedtime_tick_clock * tick_length;

      tick_counter++;

      if (tick_counter > fts_pause_period)
	{
	  fts_pause(); /* call the OS dependent pause function */
	  tick_counter = 0;
	}
    }
}


/* Tick length handling */
float fts_sched_get_tick_length(void)
{
  return tick_length;
}


static void fts_sched_set_sampling_rate(void *listener, fts_symbol_t name, const fts_atom_t *value)
{
  if (fts_is_number(value))
    {
      float f;

      f = fts_get_number_float(value);
      tick_length =  ((MAXVS * 1000) / f); /* set the scheduler tick */
    }
}


/* Pause handling  */
void fts_sched_set_pause(int p)
{
  fts_pause_period = p;
}


/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */


#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "sys.h"
#include "runtime/thread.h"

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

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

#ifndef _FTS_SCHED_H_
#define _FTS_SCHED_H_

#include "lang.h"

extern fts_module_t fts_sched_module;

/*****************************************************************************
 *
 *  ticks and msecs
 *
 */

#define FTS_MIN_TICK_SIZE 8
#define FTS_MAX_TICK_SIZE 512
#define FTS_DEF_TICK_SIZE 64

extern double fts_sched_msecs;
extern double fts_sched_ticks;
extern int fts_sched_tick_size;
extern double fts_sched_tick_duration;

#define fts_get_time_in_msecs() (fts_sched_msecs)
#define fts_get_time_in_ticks() (fts_sched_ticks)
#define fts_get_tick_size() (fts_sched_tick_size)
#define fts_get_tick_duration() (fts_sched_tick_duration)

/* 
   Description:
     The fts_sched_t abstraction contains all the data that are relevant for the
     scheduling.
     The first implementation keeps only the file descriptor set.

     Basically, each thread has its own fts_sched_t instance. As there is yet
     only one thread, there is one fts_sched_t instance.
*/
typedef struct _fts_sched fts_sched_t;

/*
   Function: fts_sched_get_current
   Description:
     Returns a pointer to the fts_sched_t that was running the calling function.
   Returns: a fts_sched_t instance that identifies the sched calling this function 
   NOTE:
     The current implementation returns a pointer to a static structure.
*/
extern fts_sched_t *fts_sched_get_current( void);

typedef void (*fts_sched_fd_fun_t)( int fd, void *data);

/*
  Function: fts_sched_add_fd
  Description:
    Add a file descriptor to the file descriptor set of the sched and declare a method
    that will be called when the file descriptor is set (i.e. a read() or write() call will not
    block).
  Arguments:
    sched: the sched owning the file descriptor set to which the file descriptor will be added
    fd: the file descriptor
    read: if one, the file descriptor is added to the read set, else to the write set
    method: the method to call when file descriptor is set
    object: the object on which to call the method
  Returns: nothing.
*/
extern void fts_sched_add_fd( fts_sched_t *sched, int fd, int read, fts_method_t method, fts_object_t *object);

/*
  Function: fts_sched_remove_fd
  Description:
    Remove a file descriptor from the file descriptor set of the sched.
  Arguments:
    sched: the sched owning the file descriptor set to which the file descriptor will be removed
    fd: the file descriptor
  Returns: nothing.
*/
extern void fts_sched_remove_fd( fts_sched_t *sched, int fd);

/*
  Function: fts_sched_add
  Description:
    Add to the sched a method that will be at each scheduler run.
  Arguments:
    sched: the sched 
    method: the method to call
    object: the object on which to call the method
  Returns: nothing.
*/
extern void fts_sched_add( fts_sched_t *sched, fts_method_t method, fts_object_t *object);

/*
  Function: fts_sched_remove
  Description:
    Remove from the sched a method that has been added with fts_sched_add
  Arguments:
    sched: the sched 
    object: the object to remove
  Returns: nothing.
*/
extern void fts_sched_remove( fts_sched_t *sched, fts_object_t *object);

/* run time */
extern void fts_sched_run(void);
extern void fts_halt(void);

extern void fts_sched_set_pause(int p);

#endif

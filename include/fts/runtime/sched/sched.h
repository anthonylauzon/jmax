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

#include <fts/lang.h>
#include <fts/runtime/time.h>

/**
 * The FTS scheduler abstraction
 *
 * Basically, each thread has its own fts_sched_t instance. 
 * As there is yet only one thread, there is one fts_sched_t instance.
 * The scheduler of a the current thread can be accessed by the function fts_sched_get_current().
 *
 * @defgroup sched scheduler
 */

/*************************************************************************
 *
 *  scheduler
 *
 */
/** 
 * @name The FTS scheduler structure
 */
/*@{*/
/**
 * The FTS scheduler structure.
 *
 * @typedef fts_sched_t
 *
 * @ingroup sched
 */

typedef struct fd_callback
{
  int fd;
  int read; /* if 1, the file descriptor will be added to the read fdset, if not to the write fd set */
  fts_method_t method;
  fts_object_t *object;
  struct fd_callback *next;
} fd_callback_t;

typedef struct fts_sched
{
  fd_callback_t *fd_callback_head; 
  enum {sched_ready, sched_halted} status;
  fts_clock_t clock; /* clock of logical time */
} fts_sched_t;


/*@}*/ /* The FTS scheduler structure */

/*************************************************************************
 *
 *  scheduler of current thread
 *
 */
/** 
 * @name Scheduler of the current thread
 */
/*@{*/

/**
 * Get logical time of the current thread in milliseconds
 *
 * @fn double fts_get_time(void)
 * @return logical time in msecs
 *
 * @ingroup sched
 */
extern double fts_get_time(void);

/**
 * Get a pointer to the scheduler (fts_sched_t) of the current thread
 *
 * @fn fts_sched_t *fts_sched_get_current(void)
 * @return scheduler of the current thread
 *
 * @ingroup sched
 */
extern fts_sched_t *fts_sched_get_current(void);

/*@}*/ /* current thread */

/** 
 * @name Handling scheduler callbacks
 *
 * Functions to add callback methods to a scheduler.
 */
/*@{*/

/**
 * Add a file descriptor to the file descriptor set of the sched and declare a method
 * that will be called when the file descriptor is set (i.e. a read() or write() call 
 * will not block).
 *
 * @fn fts_sched_add_fd(fts_sched_t *sched, int fd, int read, fts_method_t method, fts_object_t *object)
 * @param sched the sched owning the file descriptor set to which the file descriptor will be added
 * @param fd the file descriptor
 * @param read if one, the file descriptor is added to the read set, else to the write set
 * @param method the method to call when file descriptor is set
 * @param object the object on which to call the method
 *
 * @ingroup sched
 */
extern void fts_sched_add_fd(fts_sched_t *sched, int fd, int read, fts_method_t method, fts_object_t *object);

/**
 * Remove a file descriptor from the file descriptor set of the sched.
 *
 * @fn fts_sched_remove_fd(fts_sched_t *sched, int fd)
 * @param sched the sched owning the file descriptor set to which the file descriptor will be removed
 * @param fd the file descriptor
 *
 * @ingroup sched
 */
extern void fts_sched_remove_fd( fts_sched_t *sched, int fd);

/**
 * Add to the sched a method that will be at each scheduler cycle.
 *
 * @fn fts_sched_add( fts_sched_t *sched, fts_method_t method, fts_object_t *object)
 * @param sched the schedudler
 * @method the method to call
 * @param the object on which to call the method
 *
 * @ingroup sched
 */
extern void fts_sched_add( fts_sched_t *sched, fts_method_t method, fts_object_t *object);

/**
 * Remove from the sched a method that has been added with fts_sched_add.
 *
 * @fn fts_sched_remove( fts_sched_t *sched, fts_object_t *object)
 * @param sched the scheduler
 * @param object the object to remove
 * @ingroup sched
 */
extern void fts_sched_remove( fts_sched_t *sched, fts_object_t *object);

/*@}*/ /* Handling scheduler callbacks */

/* system macros */
#define fts_sched_get_clock(s) (&(s)->clock)
#define fts_sched_is_halted(s) ((s)->status == sched_halted)

/* API functions defined as macros */
#define fts_sched_advance_clock(s, t) (fts_clock_advance(&(s)->clock, (t)))
#define fts_sched_set_time(s, t) (fts_clock_set_time(&(s)->clock, (t)))

/* system functions */
extern void fts_sched_init(fts_sched_t *sched);
extern void fts_sched_run(fts_sched_t *sched);
extern void fts_sched_halt(fts_sched_t *sched);

#endif

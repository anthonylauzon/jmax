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


/**
 * The FTS scheduler abstraction
 *
 * @defgroup sched scheduler
 */

/*************************************************************************
 *
 *  scheduler
 *
 */


/** 
 * @name Handling scheduler callbacks
 *
 * Functions to add callback methods to a scheduler.
 */
/*@{*/


#define FTS_SCHED_ALWAYS 1
#define FTS_SCHED_READ 2
#define FTS_SCHED_WRITE 3

/**
 * Add an object to the scheduler
 *
 * If flags is FTS_SCHED_READ or FTS_SCHED_WRITE, third argument is a file descriptor
 * (an int) and the object will be notified when the file descriptor is set in the 
 * corresponding file descriptor set of the select() system call (i.e. a read() or 
 * write() call will not block).
 *
 * If flags is FTS_SCHED_ALWAYS, object will be notified at each scheduler tick.
 *
 * An object is notified by a "sched_ready" message. A method for this message
 * must therefore be defined.
 *
 * The object will be also notified of an error on the file descriptor that was
 * passed to fts_sched_add; this error will be notified by a "sched_error" message. 
 * Similarly, a method must have been defined.
 *
 * An example of use:
 * In class instantiation:
 * <TT>
 * fts_method_define( cl, fts_system_inlet, fts_s_sched_ready, mth_ready);
 * fts_method_define( cl, fts_system_inlet, fts_s_sched_error, mth_error);
 * </TT>
 * In init method:
 * <TT>
 * socket = socket(...);
 * fts_sched_add( (fts_object_t *)this, FTS_SCHED_READ, socket);
 * </TT>
 *
 * @fn int fts_sched_add( fts_object_t *obj, int flags, ...)
 * @param obj the object that will be notified
 * @param flags is one of FTS_SCHED_ALWAYS, FTS_SCHED_READ or FTS_SCHED_WRITE
 * @return -1 if an error occured, 0 if not
 *
 * @ingroup sched
 */
FTS_API int fts_sched_add( fts_object_t *obj, int flags, ...);


/**
 * Remove from the sched an object that has been added with fts_sched_add.
 *
 * @fn int fts_sched_remove( fts_object_t *object)
 * @param object the object to remove
 * @return -1 if an error occured, 0 if not
 * @ingroup sched
 */
FTS_API int fts_sched_remove( fts_object_t *obj);

/*@}*/ /* Handling scheduler callbacks */

/* system functions */
FTS_API void fts_sched_run(void);
FTS_API void fts_sched_halt(void);

FTS_API void fts_sleep(void);

#ifdef USE_JACK
FTS_API void fts_sched_run_one_tick(void);
FTS_API void fts_sched_run_one_tick_without_select(void);
#endif /* USE_JACK */

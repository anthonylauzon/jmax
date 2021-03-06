/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

/**
 * The FTS timing system
 *
 * An FTS timebase is a monotonous time base. A timebase can be created and advanced.
 * Alarms can be registered for a user created timebase or the main timebase of a thread
 * with a given absolute time or a delay from the current time.
 *
 * Advancing a timebase to a certain time will fire all pending alarms until the given time.
 *
 * Time is a double and in milliseconds.
 *
 * @defgroup time the timing system
 */

/*********************************************************************
 *
 *  timebase
 *
 */

/** 
 * @name timebase
 */
/*@{*/

/**
 * The FTS timebase structure.
 *
 * @typedef fts_timebase_t
 *
 * @ingroup time
 */

typedef struct fts_timebase fts_timebase_t;
typedef struct fts_timebase_entry fts_timebase_entry_t;

struct fts_timebase_entry
{
  double time; /* when to trigger this entry */
  fts_object_t *object;
  fts_method_t method; /* entry callback method */
  fts_atom_t atom; /* entry callback argument */
  fts_timebase_entry_t *next; /* next entry in timebase */
};

#define fts_timebase_entry_get_object(e) (&(e)->object)
#define fts_timebase_entry_get_method(e) (&(e)->method)
#define fts_timebase_entry_get_atom(e) (&(e)->atom)
#define fts_timebase_entry_get_time(e) (&(e)->time)

FTS_API void fts_timebase_entry_reset(fts_timebase_entry_t *entry);
FTS_API void fts_timebase_entry_set(fts_timebase_entry_t *entry, fts_object_t *object, fts_method_t method, const fts_atom_t *atom, double time);

struct fts_timebase
{ 
  fts_object_t head;

  /* time base parameters */
  double tick_time; /* tick time */
  double time; /* logical time */
  double step; /* tick step */

  fts_timebase_entry_t *entries; /* list of schedule entries */

  /* master timebase */
  fts_timebase_t *slaves; /* list of slaves */

  /* derived timebase (slave) */
  fts_timebase_t *origin; /* "ur"-timebase (master of master of ...)*/
  fts_timebase_t *master; /* master timebase */

  fts_method_t tick; /* tick method */
  fts_method_t locate; /* locate method */

  fts_timebase_t *next; /* next slave in list of master */
};

/*@}*/ /* timebase */

#define fts_timebase_is_slaved(t) ((t)->master)

#define fts_timebase_get_time(t) ((t)->time)
#define fts_timebase_set_time(t, x) ((t)->time = (x))

#define fts_timebase_get_tick_time(t) ((t)->tick_time)

#define fts_timebase_get_step(t) ((t)->step)
#define fts_timebase_set_step(t, x) ((t)->step = (x))

#define fts_timebase_get_master(t) ((t)->master)

/**
 * Initialize a timebase
 *
 * @fn void fts_timebase_init(fts_timebase_t *timebase)
 * @param timebase the timebase to initialize
 *
 * @ingroup time
 */
FTS_API void fts_timebase_init(fts_timebase_t *timebase);

/**
 * Reset a timebase
 *
 * @fn void fts_timebase_reset(fts_timebase_t *timebase)
 * @param timebases the timebase to reset
 *
 *@ingroup time 
 */
FTS_API void fts_timebase_reset(fts_timebase_t *timebase);

/***************************************************
 *
 *  timebase method scheduling
 *
 */

/**
 * Add a callback in a given timebase
 *
 * The call will be:
 * @code
 method(object, 0, 0, 1, a)
 * @endcode
 * 
 * @fn void fts_timebase_add_call(fts_timebase_t *timebase, fts_object_t *object, fts_method_t method, const fts_atom_t *a, double delay)
 * @param timebase the timebase to use
 * @param object the object
 * @param method the method to call 
 * @param a the argument of the method (only 1 argument)
 * @param delay delay in milliseconds
 *
 * @ingroup time
 */
FTS_API void fts_timebase_add_call(fts_timebase_t *timebase, fts_object_t *object, fts_method_t method, const fts_atom_t *a, double delay);

/**
 * Remove an object from a timebase
 *
 * @fn void fts_timebase_remove_object(fts_timebase_t *timebase, fts_object_t *object)
 * @param timebase the timebase
 * @param the object
 *
 * @ingroup time
 */
FTS_API void fts_timebase_remove_object(fts_timebase_t *timebase, fts_object_t *object);
FTS_API void fts_timebase_flush_object(fts_timebase_t *timebase, fts_object_t *object);


/*********************************************************************
 *
 *  derived timebases (slaves)
 *
 */

FTS_API void fts_timebase_add_slave(fts_timebase_t *timebase, fts_timebase_t *slave);
FTS_API void fts_timebase_remove_slave(fts_timebase_t *timebase, fts_timebase_t *slave);
FTS_API void fts_timebase_advance_slaves(fts_timebase_t *timebase);


/*********************************************************************
 *
 *  origin time base
 *
 */

/**
 * Advance the given timebase
 *
 * @fn void fts_timebase_advance(fts_timebase_t *timebase)
 * @param timebase
 *
 * @ingroup time
 */
FTS_API void fts_timebase_advance(fts_timebase_t *timebase);
FTS_API void fts_timebase_locate(fts_timebase_t *timebase);

/** 
 * @name logical time
 */
/*@{*/

/**
 * Get logical time in milliseconds
 *
 * @fn double fts_get_time(void)
 * @return logical time in msecs
 *
 * @ingroup time
 */

FTS_API double fts_get_time(void);
/**
 * Get main timebase 
 *
 * @fn fts_timebase_t *fts_get_timebase(void)
 * @return main timebase
 *
 * @ingroup time
 */
FTS_API fts_timebase_t *fts_get_timebase(void);

/*@}*/ /* logical time */

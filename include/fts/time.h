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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _FTS_TIME_H
#define _FTS_TIME_H

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

/*@}*/ /* logical time */

/*********************************************************************
 *
 *  timebase
 *
 */

/** 
 * @name timebase and timers
 */
/*@{*/

/**
 * The FTS timebase structure.
 *
 * @typedef fts_timebase_t
 *
 * @ingroup time
 */

/**
 * The FTS timer structure.
 *
 * @typedef fts_timer_t
 *
 * @ingroup time
 */

typedef struct fts_timebase fts_timebase_t;
typedef struct fts_timer fts_timer_t;

/**
 * Initializes timebase structure.
 *
 * @fn void fts_timebase_init(fts_timebase_t *timebase)
 * @param timebase the timebase
 *
 * @ingroup time
 */
FTS_API void fts_timebase_init(fts_timebase_t *timebase, double tick);

/**
 * Reset timebase to zero and delete all active alarms and timers.
 *
 * @fn void fts_timebase_reset(fts_timebase_t *timebase)
 * @param timebase the timebase
 *
 * @ingroup time
 */
FTS_API void fts_timebase_reset(fts_timebase_t *timebase);

/**
 * Get current timebase time.
 *
 * @fn double fts_timebase_get_time(fts_timebase_t *timebase)
 * @param timebase the timebase
 * @return current (logical) time
 *
 * @ingroup time
 */
FTS_API double fts_timebase_get_time(fts_timebase_t *timebase);

/**
 * Set current timebase time.
 *
 * @fn void fts_timebase_set_time(fts_timebase_t *timebase, double time)
 * @param timebase the timebase
 * @param time new time
 *
 * @ingroup time
 */
FTS_API void fts_timebase_set_time(fts_timebase_t *timebase, double time);

/**
 * Advance timebase one tick
 *
 * @fn void fts_timebase_tick(fts_timebase_t *timebased)
 * @param timebase the timebase
 * @param step time step to advance by
 *
 * @ingroup time
 */

FTS_API void fts_timebase_tick(fts_timebase_t *timebase);

/**
 * Set global master timebase
 *
 * @fn void fts_time_set_timebase(fts_timebase_t *timebase)
 * @param timease the timebase
 *
 * @ingroup time
 */
FTS_API void fts_time_set_timebase(fts_timebase_t *timebase);

/*@}*/ /* timebase */

/*********************************************************************
 *
 *  timer
 *
 */

struct fts_timer
{
  fts_timebase_t *timebase; /* the timers timebase */
  fts_object_t *object; /* its object */
  fts_method_t tick; /* tick callback */
  fts_method_t alarm; /* alarm callback */
  fts_method_t locate; /* locate callback */
  int active; /* timer is active */
  int n_alarms; /* number of active alarms */
  struct fts_timer *next; /* timebase list of timers */
};

#define fts_timer_get_timebase(t) ((t)->timebase)
#define fts_timer_is_active(t) ((t)->active)
#define fts_timer_has_alarm(t) ((t)->n_alarms > 0)

FTS_API fts_timer_t *fts_timer_new(fts_object_t *object, fts_timebase_t *timebase);
FTS_API void fts_timer_delete(fts_timer_t *timer);

FTS_API void fts_timer_activate(fts_timer_t *timer);
FTS_API void fts_timer_set_alarm(fts_timer_t *timer, double time, fts_atom_t *atom);
FTS_API void fts_timer_set_delay(fts_timer_t *timer, double delay, fts_atom_t *atom);
FTS_API void fts_timer_reset(fts_timer_t *timer);
FTS_API void fts_timer_flush(fts_timer_t *timer);

#endif

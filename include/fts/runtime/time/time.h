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
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#ifndef _FTS_TIME_H
#define _FTS_TIME_H

/**
 * The FTS timing system
 *
 * An FTS clock is a monotonous time base. A clock can be created and advanced.
 * Alarms can be registered for a user created clock or the main clock of a thread
 * with a given absolute time or a delay from the current time.
 *
 * Advancing a clock to a certain time will fire all pending alarms until the given time.
 *
 * Time is a double and in milliseconds.
 *
 * @defgroup time clocks, alarms and timers
 */

/**
 * The FTS clock structure.
 *
 * @typedef fts_clock_t
 *
 * @ingroup time
 */

/**
 * The FTS alarm structure.
 *
 * @typedef fts_alarm_t
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

typedef struct fts_clock fts_clock_t;
typedef struct fts_alarm fts_alarm_t;
typedef struct fts_timer fts_timer_t;

/*********************************************************************
 *
 *  clock
 *
 */

/** 
 * @name clocks
 */
/*@{*/

struct fts_clock
{
  double time; /* logical time */
  fts_alarm_t *alarms; /* list of alarms */
};

/**
 * Initializes clock structure.
 *
 * @fn void fts_clock_init(fts_clock_t *clock)
 * @param clock the clock
 *
 * @ingroup time
 */
extern void fts_clock_init(fts_clock_t *clock);

/**
 * Reset clock to zero and reset all active alarms.
 *
 * @fn void fts_clock_reset(fts_clock_t *clock)
 * @param clock the clock
 *
 * @ingroup time
 */
extern void fts_clock_reset(fts_clock_t *clock);

/**
 * Get current clock time.
 *
 * @fn double fts_clock_get_time(fts_clock_t *clock)
 * @param clock the clock
 * @return current (logical) time
 *
 * @ingroup time
 */
extern double fts_clock_get_time(fts_clock_t *clock);

/**
 * Set current clock time (doesn't fire alarms).
 *
 * @fn void fts_clock_set_time(fts_clock_t *clock, double time)
 * @param clock the clock
 * @param time new time
 *
 * @ingroup time
 */
extern void fts_clock_set_time(fts_clock_t *clock, double time);

/**
 * Advance clock to given time and sequencially fire pending alarms set to the given time or earlier.
 *
 * When firing an alarm the clock time is set to the alarm time so that all method calls have access 
 * to the current alarm time of the clock. After all alarms are fired the clock time is set to the
 * given value.
 *
 * @fn void fts_clock_advance(fts_clock_t *clock, double time)
 * @param clock the clock
 * @param time given time
 *
 * @ingroup time
 */
extern void fts_clock_advance(fts_clock_t *clock, double time);

/*@}*/ /* clocks */

/* clock functions defined as macros */
#define fts_clock_set_time(c, t) ((c)->time = t)
#define fts_clock_get_time(c) ((c)->time)


/*********************************************************************
 *
 *  alarm
 *
 */

/** 
 * @name alarms
 */
/*@{*/

struct fts_alarm
{
  double time; /* when to trigger this alarm */

  void (* fun)(fts_alarm_t *, void *); /* the function to call when the alarm trigger */
  void *arg; /* the argument to pass to the function */
  fts_alarm_t *next; /* next alarm for the same clock */
  int active; /* non zero when inserted in a list */
  fts_clock_t *clock; /* a pointer to the clock to use with this alarm */
};

#define fts_alarm_get_clock(a) ((a)->clock)

/**
 * Initialize alarm structure.
 *
 * @fn fts_alarm_t *fts_alarm_init(fts_alarm_t *alarm, fts_clock_t *clock, void (* fun)(fts_alarm_t *, void *), void *arg)
 * @param alarm the alarm structure (pointer to)
 * @param clock the clock the alarm is associcated to
 * @param fun the alarm callback function (called with the alarm and an argument when alarm goes off)
 * @param arg the argument given to the alarms callback function
 *
 * @ingroup time
 */
extern void fts_alarm_init(fts_alarm_t *alarm, fts_clock_t *clock, void (* fun)(fts_alarm_t *, void *), void *arg);

/**
 * Create and initialize new alarm.
 *
 * @fn fts_alarm_t *fts_alarm_new(fts_clock_t *clock, void (* fun)(fts_alarm_t *, void *), void *arg)
 * @param clock the clock the alarm is associcated to
 * @param fun the alarm callback function (called with the alarm and an argument when alarm goes off)
 * @param arg the argument given to the alarms callback function
 * @return new alarm
 *
 * @ingroup time
 */
extern fts_alarm_t *fts_alarm_new(fts_clock_t *clock, void (* fun)(fts_alarm_t *, void *), void *arg);

/**
 * Free alarm created with fts_alarm_new().
 *
 * @fn void fts_alarm_free(fts_alarm_t *alarm)
 * @param alarm the alarm
 *
 * @ingroup time
 */
extern void fts_alarm_free(fts_alarm_t *alarm);

/**
 * Check if alarm is activated.
 *
 * @fn int fts_alarm_is_active(fts_alarm_t *alarm)
 * @param alarm the alarm
 * @return non-zero if alarm is active
 *
 * @ingroup time
 */

extern int fts_alarm_is_active(fts_alarm_t *alarm);

/**
 * Activate alarm for given time.
 *
 * If the function is called on an alarm which is already active the alarm is reset 
 * before it is reactivated for the given time.
 *
 * @fn int fts_alarm_set_time(fts_alarm_t *alarm, double time)
 * @param alarm the alarm
 * @param time absolute time when the alarm will go off
 *
 * @ingroup time
 */
extern void fts_alarm_set_time(fts_alarm_t *alarm, double time);

/**
 * Activate alarm for given delay.
 *
 * If the function is called on an alarm which is already active the alarm is reset 
 * before it is reactivated for the given delay.
 *
 * @fn int fts_alarm_set_delay(fts_alarm_t *alarm, double delay)
 * @param alarm the alarm
 * @param delay delay time after which the alarm will go off
 *
 * @ingroup time
 */
extern void fts_alarm_set_delay(fts_alarm_t *alarm, double delay);

/**
 * Reset and desactivate alarm.
 *
 * The function has no effect on non active alarms.
 *
 * @fn int fts_alarm_reset(fts_alarm_t *alarm)
 * @param alarm the alarm
 *
 * @ingroup time
 */
extern void fts_alarm_reset(fts_alarm_t *alarm);

/*@}*/ /* alarms */

/* function defined as macro */
#define fts_alarm_is_active(a) ((a)->active == 1)

/* deprecated */
#define fts_alarm_arm(a) /* useless */
#define fts_alarm_unarm(a) fts_alarm_reset(a)
#define fts_alarm_is_in_future(a) (fts_alarm_is_active(a))
#define fts_alarm_is_armed(a) (fts_alarm_is_active(a))


/*********************************************************************
 *
 *  timer
 *
 */

/** 
 * @name timers
 */
/*@{*/

struct fts_timer
{
  fts_clock_t *clock;
  int running;
  double start;
  double time;
};

/**
 * Initialize timer structure.
 *
 * @fn fts_timer_t *fts_timer_init(fts_timer_t *timer, fts_clock_t *clock)
 * @param timer the timer structure (pointer to)
 * @param clock the clock the timer is associcated to
 *
 * @ingroup time
 */
extern void fts_timer_init(fts_timer_t *timer, fts_clock_t *clock);

/**
 * Create and initialize new timer.
 *
 * @fn fts_timer_t *fts_timer_new(fts_clock_t *clock)
 * @param clock the clock the timer is associcated to
 * @return new timer
 *
 * @ingroup time
 */
extern fts_timer_t *fts_timer_new(fts_clock_t *clock);

/**
 * Free timer created with fts_timer_new().
 *
 * @fn void fts_timer_free(fts_timer_t *timer)
 * @param timer the timer
 *
 * @ingroup time
 */
extern void fts_timer_free(fts_timer_t *timer);

/**
 * Get current timer time.
 *
 * @fn double fts_timer_get_time(fts_timer_t *timer)
 * @param timer the timer
 * @return current time
 *
 * @ingroup time
 */
extern double fts_timer_get_time(fts_timer_t *timer);

/**
 * Start timer.
 *
 * @fn double fts_timer_start(fts_timer_t *timer)
 * @param timer the timer
 *
 * @ingroup time
 */
extern void fts_timer_start(fts_timer_t *timer);

/**
 * Stop timer.
 *
 * @fn double fts_timer_stop(fts_timer_t *timer)
 * @param timer the timer
 *
 * @ingroup time
 */
extern void fts_timer_stop(fts_timer_t *timer);

/**
 * Reset timer to zero.
 *
 * @fn double fts_timer_reset(fts_timer_t *timer)
 * @param timer the timer
 *
 * @ingroup time
 */
extern void fts_timer_reset(fts_timer_t *timer);

/*@}*/ /* timers */

/* timer function defined as macros */
#define fts_timer_get_time(t) (((t)->running)? ((t)->time + ((t)->clock->time - (t)->start)): ((t)->time))

#endif

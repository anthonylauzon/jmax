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
 */

#ifndef _FTS_PRIVATE_TIME_H_
#define _FTS_PRIVATE_TIME_H_

typedef struct _alarm_
{
  fts_timer_t *timer; /* its timer */
  double time; /* when to trigger this alarm */
  fts_atom_t atom;
  struct _alarm_ *next; /* next alarm for the same timebase */
} alarm_t;

struct fts_timebase
{ 
  fts_object_t head;
  fts_timebase_t *master; /* master timebase */

  double time; /* logical time */
  double step; /* tick step */

  fts_timer_t *silent; /* list of silent timers timers */
  fts_timer_t *locate; /* list of silent locatable timers timers */
  fts_timer_t *active; /* list of active timers */

  alarm_t *alarms; /* list of alarms */
};

#endif


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

#ifndef _EXPLODE_H
#define _EXPLODE_H

#include <fts/fts.h>

#ifdef WIN32
#if defined(EXPLODE_EXPORTS)
#define EXPLODE_API __declspec(dllexport)
#else
#define EXPLODE_API __declspec(dllimport)
#endif
#else
#define EXPLODE_API extern
#endif

EXPLODE_API void
explode_config(void);

typedef struct _evt
{
  struct _evt *next;
  long time;
  long pit;
  long vel;
  long dur;
  long chan;
} evt_t;

#define NPARAM 5	/* max number of parameters */

/* definitions for score follower */

typedef struct _skip
{
  evt_t *evt;
  struct _skip *next;

} skip_t;

#define NFWD 2		/* default values for f_nfwd and f_ftime */
#define FTIME 150
#define MAXFOLLOWHANG 16

/* note-off matching for record */

typedef struct _hang
{
  evt_t *evt;
  struct _hang *next;
} hang_t;

/* WARNING -- the "explode" structure will probably change to allow
parameters to be added to the "event" structure.  I'll keep x_serial,
x_evt, x_sym, and x_next constant, though.  */

typedef struct explode_data
{
  fts_symbol_t name;		/* explode's name */
  evt_t *evt;			/* list of events */
} explode_data_t;


typedef struct 
{
  fts_object_t obj;

  explode_data_t data;

  long serial;			/* support for explay class */

  long n1;			/* values of inlets */
  long n2;
  long n3;
  long n4;

  char set1;
  char set2;
  char set3;
  long rectime;		/* logical time at which recording started */


  evt_t *current;

  int open; /* flag: is 1 if explode editor is open */

  /* score follower state */

  long matchscoretime;
  char oct;
  int nfwd;
  int ftime;
  int nhang;			/* number of hanging notes during score-follow */
  skip_t *skip;
  hang_t *hang;
  char mode;			/* defs below */
} explode_t;

#define MINIT           0
#define MPLAY           1
#define MRECORD         2
#define MFOLLOW         3


/* finding an explode by name */

extern explode_t *explode_get_by_name(fts_symbol_t name);

#define explode_set_editor_open(e) ((e)->open = 1)
#define explode_set_editor_close(e) ((e)->open = 0)
#define explode_editor_is_open(e) ((e)->open != 0)

#endif /* _EXPLODE_H */

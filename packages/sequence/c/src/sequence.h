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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette.
 *
 */



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

/* WARNING -- the "sequence" structure will probably change to allow
parameters to be added to the "event" structure.  I'll keep x_serial,
x_evt, x_sym, and x_next constant, though.  */

typedef struct sequence_data
{
  fts_data_t dataobj;
  fts_symbol_t name;		/* sequence's name */
  evt_t *evt;			/* list of events */
} sequence_data_t;


typedef struct 
{
  fts_object_t obj;

  sequence_data_t data;

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

  /* score follower state */

  long matchscoretime;
  char oct;
  int nfwd;
  int ftime;
  int nhang;			/* number of hanging notes during score-follow */
  skip_t *skip;
  hang_t *hang;
  char mode;			/* defs below */
} sequence_t;

#define MINIT           0
#define MPLAY           1
#define MRECORD         2
#define MFOLLOW         3


/* finding an sequence by name */

extern sequence_t *sequence_get_by_name(fts_symbol_t name);



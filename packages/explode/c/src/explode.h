/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
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

/* WARNING -- the "explode" structure will probably change to allow
parameters to be added to the "event" structure.  I'll keep x_serial,
x_evt, x_sym, and x_next constant, though.  */

typedef struct explode_data
{
  fts_data_t dataobj;
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



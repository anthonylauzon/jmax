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

/* Make a note (note-on/note-off events). */

#include "fts.h"

/* A simplified version of the Miller Puckette code */

/* 
   linknote is a structure used to keep a double linked list
   of active notes (i.e. notes between their on and their off)
*/

struct linknote
{
  long pitch;
  void *who;
  fts_alarm_t alarm;

  struct linknote *next;
  struct linknote *prev;
};

typedef struct 
{
  fts_object_t ob;
  long vel;
  float dur;

  fts_symbol_t clock;
  struct linknote *active_notes;
} makenote_t;


static void
linknote_send_off(fts_alarm_t *alarm, void *o)
{
  struct linknote *l = o;
  makenote_t *x = (makenote_t *) l->who;

  fts_alarm_unarm(alarm);	/* unarm the alarm, one shot only here */

  fts_outlet_int((fts_object_t *)x, 1, (long) 0);
  fts_outlet_int((fts_object_t *)x, 0, l->pitch);

  /* update the prev element; if the first in the list,
     change also the active notes pointer */

  if (l->prev)
    l->prev->next = l->next;
  else
    x->active_notes = l->next;

  /* update the next element, if any */

  if (l->next)
    l->next->prev = l->prev;

  fts_free(l);
}

/* The MakeNote object */

static void
makenote_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *x = (makenote_t *)o;
  long n = fts_get_float_arg(ac, at, 0, 0.0f);
  struct linknote *newnote;

  /* send the output messages */

  fts_outlet_int(o, 1, x->vel);
  fts_outlet_int(o, 0, n);

  /* Store the note in the active_notes list */

  newnote = (struct linknote *)fts_malloc(sizeof(struct linknote));
  newnote->who   = x;
  newnote->pitch = n;

  if (x->active_notes)
    x->active_notes->prev = newnote;

  newnote->next   = x->active_notes;
  x->active_notes = newnote;
  newnote->prev   = (struct linknote *) 0;

  /* activate the alarm */

  fts_alarm_init(&newnote->alarm, x->clock, linknote_send_off, newnote);
  fts_alarm_set_delay(&newnote->alarm, x->dur);
  fts_alarm_arm(&newnote->alarm);
}

static void
makenote_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *x = (makenote_t *)o;

  x->vel = fts_get_float_arg(ac, at, 0, 0.0f);
}

static void
makenote_number_2(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *x = (makenote_t *)o;

  x->dur = (float) fts_get_float_arg(ac, at, 0, 0.0f);
}

static void
makenote_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *x = (makenote_t *)o;

  if ((ac >= 3) && fts_is_number(&at[2]))
    x->dur = fts_get_float_arg(ac, at, 2, 0.0f);

  if ((ac >= 2) && fts_is_number(&at[1]))
    x->vel = fts_get_int_arg(ac, at, 1, 0);

  if ((ac >= 1) && fts_is_number(&at[0]))
    makenote_number(o, winlet, s, 1, at);
}

/* makenote stop method is also installed as method for the $delete message */

static void
makenote_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *x = (makenote_t *)o;
  struct linknote *p;

  p = x->active_notes; 

  while (p)
    {
      struct linknote *freeme;
      
      freeme = p;
      p = p->next;

      fts_alarm_unarm(&(freeme->alarm));
      fts_outlet_int(o, 0, (long) 0);
      fts_outlet_int(o, 1, freeme->pitch);

      fts_free(freeme);
    }
}

static void
makenote_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *x = (makenote_t *)o;
  struct linknote *p;

  p = x->active_notes; 

  while (p)
    {
      struct linknote *freeme;

      freeme = p;
      p = p->next;
      fts_free(freeme);
    }
}


static void
makenote_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *x = (makenote_t *)o;

  x->vel   = fts_get_int_arg(ac, at, 1, 0);
  x->dur   =  fts_get_float_arg(ac, at, 2, 0.0f);
  x->clock = fts_get_symbol_arg(ac, at, 3, 0); /* if zero, means milliseconds */

  if (x->clock && (! fts_clock_exists(x->clock)))
    post("makenote: warning: clock %s do not yet exists\n", fts_symbol_name(x->clock));

  x->active_notes = 0;
}


static fts_status_t
makenote_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[4];

  /* initialize the class */

  fts_class_init(cl, sizeof(makenote_t), 3, 2, 0); 

  /* system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int  ;
  a[2] = fts_s_int  ;
  a[3] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, makenote_init, 4, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, makenote_stop, 0, 0); /* makenote_stop is
										installed twice */
  /* Makenote methods */

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, makenote_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, makenote_number, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, makenote_number_1, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, makenote_number_1, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 2, fts_s_int, makenote_number_2, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 2, fts_s_float, makenote_number_2, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_list, makenote_list);

  fts_method_define(cl, 0, fts_new_symbol("clear"), makenote_clear, 0, 0);

  fts_method_define(cl, 0, fts_new_symbol("stop"), makenote_stop, 0, 0);

  /* Type the outlet */

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0,	fts_s_int, 1, a);
  fts_outlet_type_define(cl, 1,	fts_s_int, 1, a);

  return fts_Success;
}


void
makenote_config(void)
{
  fts_class_install(fts_new_symbol("makenote"),makenote_instantiate);
}

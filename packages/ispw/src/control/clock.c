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

/* 
   Define a FTS clock, that can be then used by all the time related
   objects.

   It need a symbol argument, giving the clock names; the absolute
   time is initialized to zero.

   It accept the following messages:

   "bang"  increment the clock
   "set" n  set the time to n.
   "zero" or "reset" set the time to zero.
   int,float     add the integer to the current absolute time.

   Don't call this object clock_t, it conflict with system types
   on many platforms.
*/

#include <fts/fts.h>

typedef struct 
{
  fts_object_t ob;
  fts_symbol_t name;
  double *time;
} clock_obj_t;


static void
clock_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  clock_obj_t *x = (clock_obj_t *)o;

  (*(x->time)) += 1.0;
}

static void
clock_reset(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  clock_obj_t *x = (clock_obj_t *)o;

  *(x->time) = 0.0;
  fts_clock_reset(x->name);
}


static void
clock_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  double new_time;
  clock_obj_t *x = (clock_obj_t *)o;

  new_time = fts_get_double_arg(ac, at, 0, 0);

  if (*(x->time) > new_time)
    fts_clock_reset(x->name);

  *(x->time) = new_time;
}


static void
clock_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  clock_obj_t *x = (clock_obj_t *)o;

  *(x->time) += fts_get_double_arg(ac, at, 0, 0);
}

/*
   Note that in the current implementation we leak memory (one allocated long)
   when we delete a clock;
*/

static void
clock_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  clock_obj_t *x = (clock_obj_t *)o;

  x->name = fts_get_symbol_arg(ac, at, 1, 0);

  if (fts_clock_exists(x->name))
    {
      x->time = fts_clock_get_real_time_p(x->name); /* not documented call !!!! */
    }
  else
    {
      x->time = (double *) fts_malloc(sizeof(double));
      fts_clock_define(x->name, x->time);
      *(x->time) = 0;
    }
}

static void
clock_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  clock_obj_t *x = (clock_obj_t *)o;

  fts_clock_undefine(x->name);
}


static fts_status_t
clock_instantiate(fts_class_t *cl, int ac, const fts_atom_t *clock_dummy)
{
  fts_symbol_t a[2];

  /* initialize the class */

  fts_class_init(cl, sizeof(clock_obj_t), 1, 0, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, clock_init, 2, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, clock_delete, 0, 0);

  /* At args */

  fts_method_define(cl, 0, fts_s_bang, clock_bang, 0, 0);
  /* fts_method_define(cl, 0, fts_new_symbol("zero"),  clock_reset, 0, 0); */
  fts_method_define(cl, 0, fts_new_symbol("reset"), clock_reset, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, clock_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, clock_number, 1, a);

  a[0] = fts_s_number;
  fts_method_define(cl, 0, fts_new_symbol("set"), clock_set, 1, a);

  return fts_Success;
}


void
clock_config(void)
{
  fts_class_install(fts_new_symbol("clock"),clock_instantiate);
}


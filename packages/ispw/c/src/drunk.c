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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette, Zack Settel.
 *
 */

#include <fts/fts.h>

#define MAXVAL	0x7FFF

typedef struct 
{
  fts_object_t ob;
  long reg;
  float scaler;
  long range;
} drunk_t;


/*  output a value  */

static void
drunk_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  extern int rand(void);
  drunk_t *x = (drunk_t *)o;
  int offset, newval;

  offset = 1 + (int)(x->scaler * (0x7FFF & rand()));

  if (x->reg > x->range)
    x->reg = x->range;
  else if (x->reg < 0)
    x->reg = 0;

  offset = (rand() & 512) ? -offset : offset;
  newval = offset + x->reg;

  if (newval < 0 || newval > x->range)
    newval = x->reg - offset;

  x->reg = newval;

  fts_outlet_int(o, 0, newval);
}


/*   change correlation   */

static void
drunk_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  drunk_t *x = (drunk_t *)o;
  long n = (long) fts_get_int_arg(ac, at, 0, 0);

  if (n < 0)
    n = 0;
  else if (n > x->range)
    n = x->range;

  x->reg = n;

  fts_outlet_int(o, 0, n);
}

/*  change range -- msp  */

static void
drunk_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  drunk_t *x = (drunk_t *)o;
  long n = (long) fts_get_int_arg(ac, at, 0, 0);

  if (n <= 0)
    n = 1;

  x->range = n;

  if (x->reg >= n)
    x->reg = n-1;
}


/*   change correlation   */

static void
drunk_number_2(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  drunk_t *x = (drunk_t *)o;
  long n = (long) fts_get_int_arg(ac, at, 0, 0);

  n = (n > x->range / 2L) ? (x->range / 2L) : n;
  x->scaler = (float)n / MAXVAL;
}


/* Method for list */
static void
drunk_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if ((ac >= 3) && fts_is_number(&at[2]))
    drunk_number(o, 2, s, 1, at + 2);

  if ((ac >= 2) && fts_is_number(&at[1]))
    drunk_number(o, 1, s, 1, at + 1);

  if ((ac >= 1) && fts_is_number(&at[0]))
    drunk_number(o, 0, s, 1, at);
}


/*   change current value (position in drunk walk)  */

static void
drunk_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  drunk_t *x = (drunk_t *)o;
  long n = (long) fts_get_int_arg(ac, at, 0, 0);

  x->reg = n % x->range;
}


static void
drunk_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  drunk_t *x = (drunk_t *)o;
  long int range       = (long) fts_get_int_arg(ac, at, 0, 0);
  long int correlation = (long) fts_get_int_arg(ac, at, 1, 0);

  if (range <= 0)
    range = 128;

  if (correlation <= 0)
    correlation = 1;

  correlation = (correlation > range / 2L) ? (range / 2L) : correlation;
  x->scaler = (float)correlation / MAXVAL;
  x->reg = range / 2L;
  x->range = range;
}


static fts_status_t
drunk_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(drunk_t), 3, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, drunk_init);

  fts_method_define_varargs(cl, 0, fts_s_set, drunk_set);

  fts_method_define_varargs(cl, 0, fts_s_bang, drunk_bang);
  fts_method_define_varargs(cl, 0, fts_s_int, drunk_number);
  fts_method_define_varargs(cl, 0, fts_s_float, drunk_number);
  fts_method_define_varargs(cl, 0, fts_s_list, drunk_list);

  fts_method_define_varargs(cl, 1, fts_s_int,   drunk_number_1);
  fts_method_define_varargs(cl, 1, fts_s_float, drunk_number_1);

  fts_method_define_varargs(cl, 2, fts_s_int,   drunk_number_2);
  fts_method_define_varargs(cl, 2, fts_s_float, drunk_number_2);

  return fts_Success;
}

void
drunk_config(void)
{
  fts_class_install(fts_new_symbol("drunk"),drunk_instantiate);
}


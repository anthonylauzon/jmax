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

#include <fts/fts.h>

typedef struct {
  fts_object_t o;
  long int_bound[2];
  float float_bound[2];
} split_t;

static void
split_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  split_t *this = (split_t *)o;
  long l = fts_get_int(at);

  if (l <= this->int_bound[1] && l >= this->int_bound[0])
    fts_outlet_int(o, 0, l);
  else
    fts_outlet_int(o, 1, l);
}

static void
split_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  split_t *this = (split_t *)o;
  float f = fts_get_float(at);

  if (f <= this->float_bound[1] && f >= this->float_bound[0])
    fts_outlet_float(o, 0, f);
  else
    fts_outlet_float(o, 1, f);
}

static void
split_bound(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  split_t *this = (split_t *)o;
  float float_bound = fts_get_float_arg(ac, at, 0, 0.0f);
  long int_bound = float_bound;

  this->int_bound[winlet-1] = ((float)int_bound == float_bound)? int_bound: int_bound + 1;
  this->float_bound[winlet-1] = float_bound;
}

static void
split_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  split_bound(o, 1, 0, 1, at + 1);
  split_bound(o, 2, 0, 1, at + 2);
}

static void
split_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  if(ac >= 1)
    {
      if(fts_is_float(at))
	split_float(o, 0, 0, 1, at);
      else if(fts_is_float(at))
	split_int(o, 0, 0, 1, at);	
    }

  if(ac >= 2)
    split_bound(o, 1, 0, 1, at + 1);

  if(ac >= 3)
    split_bound(o, 2, 0, 1, at + 2);
}

static fts_status_t
split_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(split_t), 3, 2, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, split_init);

  fts_method_define_varargs(cl, 0, fts_s_int, split_int);
  fts_method_define_varargs(cl, 0, fts_s_float, split_float);

  fts_method_define_varargs(cl, 1, fts_s_int, split_bound);
  fts_method_define_varargs(cl, 1, fts_s_float, split_bound);

  fts_method_define_varargs(cl, 2, fts_s_int, split_bound);
  fts_method_define_varargs(cl, 2, fts_s_float, split_bound);

  fts_method_define_varargs(cl, 0, fts_s_list, split_list);

  return fts_Success;
}

void
split_config(void)
{
  fts_class_install(fts_new_symbol("split"),split_instantiate);
}


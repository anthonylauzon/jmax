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
 */


#include <fts/fts.h>

/*------------------------- accum class -------------------------------------*/

typedef struct {
  fts_object_t o;
  float s_reg;
} accum_t;

static void
iaccum_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ((accum_t *)o)->s_reg = (float)fts_get_long_arg(ac, at, 0, 0);
  fts_outlet_send(o, 0, fts_s_int, ac, at);
}

static void
iaccum_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t a;
  ((accum_t *)o)->s_reg = fts_get_float(at);
  fts_set_int(&a, ((accum_t *)o)->s_reg);
  fts_outlet_send(o, 0, fts_s_int, 1, &a);
}

static void
iaccum_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t a;
  fts_set_int(&a, (long)((accum_t *)o)->s_reg);
  fts_outlet_send(o, 0, fts_s_int, 1, &a);
}

static void
faccum_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t a;
  ((accum_t *)o)->s_reg = (float)fts_get_int(at);
  fts_set_float(&a, ((accum_t *)o)->s_reg);
  fts_outlet_send(o, 0, fts_s_float, 1, &a);
}

static void
faccum_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ((accum_t *)o)->s_reg = fts_get_float(at);
  fts_outlet_send(o, 0, fts_s_float, ac, at);
}

static void
faccum_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t a;
  fts_set_float(&a, ((accum_t *)o)->s_reg);
  fts_outlet_send(o, 0, fts_s_float, 1, &a);
}

static void
accum_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ((accum_t *)o)->s_reg = fts_get_float_arg(ac, at, 0, 0);
}

static void
accum_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ((accum_t *)o)->s_reg = fts_get_float_arg(ac, at, 1, 0);
}

static void
accum_int_middle(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ((accum_t *)o)->s_reg += fts_get_long_arg(ac, at, 0, 0);
}

static void
accum_int_right(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ((accum_t *)o)->s_reg *= fts_get_long_arg(ac, at, 0, 0);
}

static void
accum_float_middle(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ((accum_t *)o)->s_reg += fts_get_float_arg(ac, at, 0, 0);
}

static void
accum_float_right(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ((accum_t *)o)->s_reg *= fts_get_float_arg(ac, at, 0, 0);
}

static fts_status_t
accum_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(accum_t), 3, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, accum_init, 2, a, 1);

  a[0] = fts_s_number;
  fts_method_define(cl, 0, fts_s_set, accum_set, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, accum_int_middle, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, accum_float_middle, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 2, fts_s_int, accum_int_right, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 2, fts_s_float, accum_float_right, 1, a);

  if ((ac <= 1) || fts_is_int(at + 1))
    {
      a[0] = fts_s_int;
      fts_method_define(cl, 0, fts_s_int, iaccum_int, 1, a);

      a[0] = fts_s_float;
      fts_method_define(cl, 0, fts_s_float, iaccum_float, 1, a);
      
      fts_method_define(cl, 0, fts_s_bang, iaccum_bang, 0, a);

      a[0] = fts_s_int;
      fts_outlet_type_define(cl, 0, fts_s_int, 1, a);
    }
  else
    {
      a[0] = fts_s_int;
      fts_method_define(cl, 0, fts_s_int, faccum_int, 1, a);

      a[0] = fts_s_float;
      fts_method_define(cl, 0, fts_s_float, faccum_float, 1, a);
      
      fts_method_define(cl, 0, fts_s_bang, faccum_bang, 0, a);

      a[0] = fts_s_float;
      fts_outlet_type_define(cl, 0, fts_s_float, 1, a);
    }

  return fts_Success;
}

void
accum_config(void)
{
  fts_metaclass_install(fts_new_symbol("accum"), accum_instantiate, fts_arg_equiv);
}


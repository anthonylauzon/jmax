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

typedef struct
{
  fts_object_t o;
} unpack_t;

static fts_symbol_t unpack_s_f = 0;
static fts_symbol_t unpack_s_i = 0;

static void
unpack_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int i;

  if (ac > fts_object_get_outlets_number(o))
    ac = fts_object_get_outlets_number(o);

  for (i = ac-1; i >= 0; i--)
    {
      fts_atom_t a;
      fts_symbol_t outlet_type;

      outlet_type = fts_object_get_outlet_type(o, i);

      if ((outlet_type == fts_s_int) && fts_is_int(at + i))
	fts_outlet_send(o, i, outlet_type, 1, at + i);
      else if ((outlet_type == fts_s_int) && fts_is_float(at + i))
	{
	  fts_set_int(&a, (int)fts_get_float(at + i));
	  fts_outlet_send(o, i, outlet_type, 1, &a);
	}
      else if ((outlet_type == fts_s_float) && fts_is_float(at + i))
	fts_outlet_send(o, i, outlet_type, 1, at + i);
      else if ((outlet_type == fts_s_float) && fts_is_int(at + i))
	{
	  fts_set_float(&a, (float)fts_get_int(at + i));
	  fts_outlet_send(o, i, outlet_type, 1, &a);
	}
      else if ((outlet_type == fts_s_symbol) && fts_is_symbol(at + i))
	fts_outlet_send(o, i, outlet_type, 1, at + i);
    }
}

static fts_status_t
unpack_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a;
  int i;

  ac--;
  at++;

  if (!ac)
    {
      fts_class_init(cl, sizeof(unpack_t), 1, 2, 0);

      fts_outlet_type_define_varargs(cl, 0, fts_s_int);
      fts_outlet_type_define_varargs(cl, 1, fts_s_int);
    }
  else
    fts_class_init(cl, sizeof(unpack_t), 1, ac, 0);

  fts_method_define_varargs(cl, 0, fts_s_list, unpack_send);
  fts_method_define_varargs(cl, 0, fts_s_int, unpack_send);
  fts_method_define_varargs(cl, 0, fts_s_float, unpack_send);

  for(i=0; i<ac; i++)
    {
      if (fts_is_float(at + i))
	fts_outlet_type_define_varargs(cl, i, fts_s_float);
      else if (fts_is_int(at + i))
	fts_outlet_type_define_varargs(cl, i, fts_s_int);
      else if (fts_is_symbol(at + i))
	{
	  fts_symbol_t sym = fts_get_symbol(at + i);

	  if(sym == unpack_s_i)
	    fts_outlet_type_define_varargs(cl, i, fts_s_int);
	  else if(sym == unpack_s_f)
	    fts_outlet_type_define_varargs(cl, i, fts_s_float);
	  else
	    fts_outlet_type_define_varargs(cl, i, fts_s_symbol);
	}
    }
  
  return fts_Success;
}

void
unpack_config(void)
{
  unpack_s_f = fts_new_symbol("f");
  unpack_s_i = fts_new_symbol("i");

  fts_metaclass_install(fts_new_symbol("unpack"),unpack_instantiate, fts_arg_type_equiv);
}

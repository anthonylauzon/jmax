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
  int n;
  enum unpack_types {type_int, type_float, type_symbol} *types;
} unpack_t;

static fts_symbol_t sym_f = 0;
static fts_symbol_t sym_i = 0;

static void
unpack_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  unpack_t *this = (unpack_t *)o;
  int i;

  if(ac > this->n)
    ac = this->n;

  for(i=ac-1; i>=0; i--)
    {
      switch(this->types[i])
	{
	case type_int:
	  if(fts_is_number(at + i))
	    fts_outlet_int(o, i, fts_get_number_int(at + i));
	  break;

	case type_float:
	  if(fts_is_number(at + i))
	    fts_outlet_float(o, i, fts_get_number_float(at + i));
	  break;

	case type_symbol:
	  if(fts_is_symbol(at + i))
	    fts_outlet_symbol(o, i, fts_get_symbol(at + i));
	  break;

	default:
	  break;
	}
    }
}

static void
unpack_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  unpack_t *this = (unpack_t *)o;
  int i;

  ac--;
  at++;

  if(!ac)
    {
      this->n = 2;
      this->types = (enum unpack_types *)fts_malloc(2 * sizeof(int));

      this->types[0] = type_int;
      this->types[1] = type_int;
    }
  else
    {
      this->n = ac;
      this->types = (enum unpack_types *)fts_malloc(ac * sizeof(int));      

      for(i=0; i<ac; i++)
	{
	  if (fts_is_int(at + i))
	    this->types[i] = type_int;
	  else if (fts_is_float(at + i))
	    this->types[i] = type_float;
	  else if (fts_is_symbol(at + i))
	    {
	      fts_symbol_t sym = fts_get_symbol(at + i);
	      
	      if(sym == sym_i)
		this->types[i] = type_int;
	      else if(sym == sym_f)
		this->types[i] = type_float;
	      else
		this->types[i] = type_symbol;
	    }
	}
    }
}

static fts_status_t
unpack_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a;
  int i;

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
      if (fts_is_number(at + i))
	fts_outlet_type_define_varargs(cl, i, fts_get_selector(at + i));
      else if (fts_is_symbol(at + i))
	{
	  fts_symbol_t sym = fts_get_symbol(at + i);

	  if(sym == sym_i)
	    fts_outlet_type_define_varargs(cl, i, fts_s_int);
	  else if(sym == sym_f)
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
  sym_f = fts_new_symbol("f");
  sym_i = fts_new_symbol("i");

  fts_metaclass_install(fts_new_symbol("unpack"), unpack_instantiate, fts_arg_type_equiv);
}

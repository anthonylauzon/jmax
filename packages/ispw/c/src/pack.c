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

/* utility function for pack: assign a value from a source atom to the
   destination atom, without changing the type of the destination
   atom */

static fts_symbol_t pack_s_f = 0;
static fts_symbol_t pack_s_i = 0;

static void
set_value_preserve_type(const fts_atom_t *src, fts_atom_t *dst)
{
  if (fts_is_int(dst))
    {
      if (fts_is_int(src))
	fts_set_int(dst, fts_get_int(src));
      else if (fts_is_float(src))
	fts_set_int(dst, (long) fts_get_float(src));
      else 
	fts_set_int(dst, 0);
    }
  else if (fts_is_float(dst))
    {
      if (fts_is_int(src))
	fts_set_float(dst, (float) fts_get_int(src));
      else if (fts_is_float(src))
	fts_set_float(dst, fts_get_float(src));
      else 
	fts_set_float(dst, 0.0f);
    }
  else if (fts_is_void(dst))
    {
      /* a void atom is a not initialized atom; just put
         whatever you receive */
      *dst = *src;
    }
  else
    {
      if (fts_is_int(src))
	fts_set_symbol(dst, (fts_symbol_t )fts_s_int);
      else if (fts_is_float(src))
	fts_set_symbol(dst, (fts_symbol_t ) fts_s_float);
      else 
	fts_set_symbol(dst, fts_get_symbol(src));
    }
}


typedef struct
{
  fts_object_t o;
  int argc;			/* used size */
  fts_atom_t *argv;
} pack_t;


static void
pack_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int i;
  pack_t *x =  (pack_t *)o;

  if (ac > x->argc)
    ac = x->argc;

  for (i = 0; i < ac; i++) 
   set_value_preserve_type(&at[i], &(x->argv[i]));

  fts_outlet_atoms(o, 0, x->argc, x->argv);
}

static void
pack_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_atoms(o, 0, ((pack_t *)o)->argc, ((pack_t *)o)->argv);
}

static void
pack_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pack_t *x = (pack_t *)o;

  if (ac > 0)
    set_value_preserve_type(&at[0], &(x->argv[0]));

  fts_outlet_atoms(o, 0, x->argc, x->argv);
}

static void
pack_inlet(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (ac > 0)
    ((pack_t *)o)->argv[winlet] = at[0];
}

static void
pack_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pack_t *x = (pack_t *)o;
  int i;

  if (! ac)
    {
      x->argc = 2;
      x->argv = (fts_atom_t *) fts_malloc(x->argc * sizeof(fts_atom_t));
      fts_set_int(&(x->argv[0]), 0);
      fts_set_int(&(x->argv[1]), 0);
    }
  else
    {
      x->argc = ac;
      x->argv = (fts_atom_t *) fts_malloc(x->argc * sizeof(fts_atom_t));

      for (i = 0; i < ac ; i++)
	{
	  /* Bizzarre compatibility hack .. */

	  if (fts_is_symbol(&at[i]) && (fts_get_symbol(&at[i]) == pack_s_i))
	    fts_set_int(&(x->argv[i]), 0);
	  else if (fts_is_symbol(&at[i]) && (fts_get_symbol(&at[i]) == pack_s_f))
	    fts_set_float(&(x->argv[i]), 0.0f);
	  else
	    x->argv[i] = at[i];
	}
    }

  fts_object_set_inlets_number(o, x->argc);  
}

static void
pack_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pack_t *x = (pack_t *)o;

  fts_free( x->argv);
}

static fts_status_t
pack_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(pack_t), 2, 1, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, pack_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, pack_delete);

  fts_method_define_varargs(cl, 0, fts_s_bang, pack_bang);

  fts_method_define_varargs(cl, 0, fts_s_list, pack_list);

  fts_method_define_varargs(cl, 0, fts_s_int, pack_send);
  fts_method_define_varargs(cl, 0, fts_s_float, pack_send);
  fts_method_define_varargs(cl, 0, fts_s_symbol, pack_send);

  fts_method_define_varargs(cl, 1, fts_s_int, pack_inlet);
  fts_method_define_varargs(cl, 1, fts_s_float, pack_inlet);
  fts_method_define_varargs(cl, 1, fts_s_symbol, pack_inlet);

  fts_outlet_type_define_varargs(cl, 0,	fts_s_list);

  return fts_ok;
}

void
pack_config(void)
{
  pack_s_f = fts_new_symbol("f");
  pack_s_i = fts_new_symbol("i");

  fts_class_install(fts_new_symbol("pack"), pack_instantiate);
}


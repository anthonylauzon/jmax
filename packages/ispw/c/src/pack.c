/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
pack_list(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  int i;
  pack_t *x =  (pack_t *)o;

  if (ac > x->argc)
    ac = x->argc;

  for (i = 0; i < ac; i++) 
   set_value_preserve_type(&at[i], &(x->argv[i]));

  fts_outlet_varargs(o, 0, x->argc, x->argv);
}

static void
pack_bang(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  pack_t* self = (pack_t*)o;
  fts_outlet_varargs(o, 0, self->argc, self->argv);
}

static void
pack_send(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  pack_t *x = (pack_t *)o;

  if (ac > 0)
    set_value_preserve_type(&at[0], &(x->argv[0]));

  fts_outlet_varargs(o, 0, x->argc, x->argv);
}

static void
pack_inlet(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  int winlet = fts_object_get_message_inlet(o);

  if (ac > 0)
    ((pack_t *)o)->argv[winlet] = at[0];
}

static void
pack_error_inlet(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  int winlet = fts_object_get_message_inlet(o);

  fts_post("pack object want number or symbol in inlet %d\n", winlet);
  fts_object_error(o, "pack object want number or symbol in inlet %d", winlet);
}

static void
pack_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
	  {
	    fts_set_int(&(x->argv[i]), 0);
	  }
	  else if (fts_is_symbol(&at[i]) && (fts_get_symbol(&at[i]) == pack_s_f))
	  {
	    fts_set_float(&(x->argv[i]), 0.0f);
	  }
	  else
	  {
	    if (fts_is_object(&at[i]))
	    {
	      fts_object_error(o, "pack only supports int and float");
	      return;
	    }
	    x->argv[i] = at[i];
	  }
	}
    }

  fts_object_set_inlets_number(o, x->argc);  
}

static void
pack_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  pack_t *x = (pack_t *)o;
  int i;

  fts_free( x->argv);
}

static void
pack_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(pack_t), pack_init, pack_delete);

  fts_class_inlet_bang(cl, 0, pack_bang);

  fts_class_inlet_varargs(cl, 0, pack_list);
  fts_class_inlet_number(cl, 0, pack_send);
  fts_class_inlet_symbol(cl, 0, pack_send);
  fts_class_inlet_number(cl, 1, pack_inlet);
  fts_class_inlet_symbol(cl, 1, pack_inlet);
  fts_class_inlet_varargs(cl, 1, pack_error_inlet);
  fts_class_outlet_varargs(cl, 0);
}

void
pack_config(void)
{
  pack_s_f = fts_new_symbol("f");
  pack_s_i = fts_new_symbol("i");

  fts_class_install(fts_new_symbol("pack"), pack_instantiate);
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */

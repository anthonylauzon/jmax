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

/********************************************
 *
 *  iroute
 *
 */

typedef struct 
{
  fts_object_t o;
  int ns;
  int *ints;
} iroute_t;

static void
iroute_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  iroute_t *this = (iroute_t *)o;
  int ns = this->ns;
  int n = fts_get_number_int(at);
  int match = 0;
  int i;

  for(i=ns-1; i>=0; i--)
    {
      if(this->ints[i] == n)
	{
	  match = 1;
	  fts_outlet_bang(o, i);
	}
    }

  if (!match)
    fts_outlet_send(o, ns, s, ac, at);
}

static void
iroute_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  iroute_t *this = (iroute_t *)o;
  int ns = this->ns;
  int n = fts_get_int_arg(ac, at, 0, 0);
  int match = 0;
  int i;

  for(i=ns-1; i>=0; i--)
    {
      if (this->ints[i] == n)
	{
	  match = 1;

	  if(ac == 1)
	    fts_outlet_bang(o, i);
	  else if (fts_is_symbol(at))
	    {
	      fts_symbol_t sel = fts_get_symbol(at);
	      
	      fts_outlet_send(o, i, sel, ac - 2, at + 2);
	    }
	  else 
	    fts_outlet_atoms(o, i, ac - 1, at + 1);
	}
    }

  if (!match)
    fts_outlet_atoms(o, ns, ac, at);
}

static void
iroute_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  iroute_t *this = (iroute_t *)o;
  int i;

  ac--;
  at++;

  this->ns = ac;
  this->ints = (int *)fts_malloc(sizeof(int) * ac);

  for(i=0; i<ac; i++)
    {
      if(fts_is_int(at + i))
	this->ints[i] = fts_get_int(at + i);
      else
	fts_object_set_error(o, "All arguments of int required");
    }
}

static void
iroute_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  iroute_t *this = (iroute_t *)o;

  fts_free(this->ints);
}

static fts_status_t
iroute_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(iroute_t), 1, ac + 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, iroute_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, iroute_delete);

  fts_method_define_varargs(cl, 0, fts_s_int, iroute_int);
  fts_method_define_varargs(cl, 0, fts_s_float, iroute_int);

  fts_method_define_varargs(cl, 0, fts_s_list, iroute_list);

  return fts_Success;
}

/********************************************
 *
 *  mroute
 *
 */

typedef struct 
{
  fts_object_t o;
  int ns;
  fts_symbol_t *symbols;
} mroute_t;

static void
mroute_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mroute_t *this = (mroute_t *)o;
  int ns = this->ns;
  int match = 0;
  int i;

  for(i=ns-1; i>=0; i--)
    {
      if (this->symbols[i] == fts_s_bang)
	{
	  match = 1;
	  fts_outlet_bang(o, i);
	}
    }

  if (!match)
    fts_outlet_bang(o, ns);
}

static void
mroute_atom(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mroute_t *this = (mroute_t *)o;
  int ns = this->ns;
  int match = 0;
  int i;

  for(i=ns-1; i>=0; i--)
    {
      if (this->symbols[i] == fts_get_selector(at))
	{
	  match = 1;
	  fts_outlet_atom(o, i, at);
	}
    }

  if (!match)
    fts_outlet_atom(o, ns, at);
}

static void
mroute_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mroute_t *this = (mroute_t *)o;
  int ns = this->ns;
  int match = 0;
  int i;

  for(i=ns-1; i>=0; i--)
    {
      if (this->symbols[i] == fts_s_list)
	{
	  match = 1;
	  fts_outlet_atoms(o, i, ac, at);
	}
    }

  if (!match)
    fts_outlet_atoms(o, ns, ac, at);
}

static void
mroute_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mroute_t *this = (mroute_t *)o;
  int ns = this->ns;
  int match = 0;
  int i;

  for(i=ns-1; i>=0; i--)
    {
      if (this->symbols[i] == s)
	{
	  match = 1;

	  if (ac == 0)
	    fts_outlet_bang(o, i);
	  else if (fts_is_symbol(at))
	    {
	      fts_symbol_t sel = fts_get_symbol(at);
	      
	      fts_outlet_send(o, i, sel, ac - 1, at + 1);
	    }
	  else
	    fts_outlet_atoms(o, i, ac, at);
	}
    }

  if (!match)
    fts_outlet_send(o, ns, s, ac, at);
}


static void
mroute_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mroute_t *this = (mroute_t *)o;
  int i;

  ac--; 
  at++;

  this->ns = ac;
  this->symbols = (fts_symbol_t *)fts_malloc(sizeof(fts_symbol_t) * ac);

  for(i=0; i<ac; i++)
    {
      if(fts_is_symbol(at + i))
	this->symbols[i] = fts_get_symbol(at + i);
      else
	fts_object_set_error(o, "All arguments of symbol required");
    }
}

static void
mroute_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mroute_t *this = (mroute_t *)o;

  fts_free(this->symbols);
}

static fts_status_t
mroute_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(mroute_t), 1, ac + 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, mroute_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, mroute_delete);

  fts_method_define_varargs(cl, 0, fts_s_bang, mroute_bang);
  fts_method_define_varargs(cl, 0, fts_s_int, mroute_atom);
  fts_method_define_varargs(cl, 0, fts_s_float, mroute_atom);
  fts_method_define_varargs(cl, 0, fts_s_list, mroute_list);

  fts_method_define_varargs(cl, 0, fts_s_anything, mroute_anything);

  return fts_Success;
}

static fts_status_t
route_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac > 0)
    {
      if(fts_is_int(at))
	return iroute_instantiate(cl, ac, at);
      else if(fts_is_symbol(at))
	return mroute_instantiate(cl, ac, at);	
    }

  return &fts_CannotInstantiate;
}

void
route_config(void)
{
  fts_metaclass_install(fts_new_symbol("route"), route_instantiate, fts_arg_type_equiv);
}

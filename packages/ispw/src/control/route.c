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

#include "fts.h"

typedef struct {
  fts_object_t o;
  int ns;
  fts_symbol_t *symb;
} mroute_t;

typedef struct {
  fts_object_t o;
  int ns;
  long *s_n;
} iroute_t;

fts_symbol_t  sym_tilde = 0;

static void
mroute_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mroute_t *x = (mroute_t *)o;
  fts_symbol_t *symb;
  int n;
  const fts_atom_t *tat;

  ac--; at++;			/* throw away class name argument */

  x->ns = ac;
  x->symb = (fts_symbol_t *)fts_malloc(sizeof(fts_symbol_t )*ac);

  for (symb = x->symb, tat = at, n = 0; n < ac; symb++, tat++, n++)
    *symb = fts_get_symbol(tat);
}



static void
mroute_realize(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int ns = ((mroute_t *)o)->ns;
  fts_symbol_t *symb = ((mroute_t *)o)->symb + ns;
  int match = 0;

  while (ns--)
    if (*(--symb) == s)
      {
	match = 1;
	fts_outlet_send(o, ns, s, ac, at);
      }

  if (!match)
    fts_outlet_send(o, ((mroute_t *)o)->ns, s, ac, at);
}

static void
mroute_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_free(((mroute_t *)o)->symb);
}

static void
mroute_send(fts_object_t *o, int ns, int ac, const fts_atom_t *at)
{
  fts_symbol_t s;

  if (!ac)
    s = fts_s_bang;
  else if (fts_is_symbol(at))
    {
      s = fts_get_symbol(at);
      ac--;
      at++;
    }
  else if (ac > 1 )
    s = fts_s_list;
  else if (fts_is_long(at))
    s = fts_s_int;
  else if (fts_is_float(at))
    s = fts_s_float;

  fts_outlet_send(o, ns, s, ac, at);
}

static void
mroute_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int ns = ((mroute_t *)o)->ns;
  fts_symbol_t *symb = ((mroute_t *)o)->symb + ns;
  int match = 0;

  while (ns--)
    if (*(--symb) == s)
      {
	match = 1;
	mroute_send(o, ns, ac, at);
      }

  if (!match)
    fts_outlet_send(o, ((mroute_t *)o)->ns, s, ac, at);
}


static void
iroute_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int ns = ((iroute_t *)o)->ns;
  long n = (long)fts_get_number_int(at), *p = ((iroute_t *)o)->s_n+ns;
  int match = 0;

  while (ns--)
    if (*(--p) == n)
      {
	match = 1;
	fts_outlet_send(o, ns, fts_s_bang, 0, 0);
      }

  if (!match)
    fts_outlet_send(o, ((iroute_t *)o)->ns, s, ac, at);
}

static void
iroute_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_free(((iroute_t *)o)->s_n);
}

static void
iroute_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  iroute_t *x = (iroute_t *)o;
  long *pn;
  int n;
  const fts_atom_t *tat;

  ac--; at++;			/* throw away class name argument */

  x->ns = ac;
  x->s_n = (long *)fts_malloc(sizeof(int)*ac);

  for (pn = x->s_n, tat = at, n = 0; n < ac; pn++, tat++, n++)
    {
      *pn = fts_get_long(tat);
    }
}

static void
iroute_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int ns = ((iroute_t *)o)->ns;
  long n = fts_get_long_arg(ac, at, 0, 0), *p = ((iroute_t *)o)->s_n+ns;
  int match = 0;

  while (ns--)
    if (*(--p) == n)
      {
	fts_symbol_t s;
	match = 1;

	ac--;
	at++;

	if (ac<0)
	  s = fts_s_bang;
	else if (fts_is_symbol(at))
	  {
	    s = fts_get_symbol(at);
	    ac--;
	    at++;
	  }
	else if (ac > 1 )
	  s = fts_s_list;
	else if (fts_is_long(at))
	  s = fts_s_int;
	else if (fts_is_float(at))
	  s = fts_s_float;

	fts_outlet_send(o, ns, s, ac, at);
      }

  if (!match)
    fts_outlet_send(o, ((iroute_t *)o)->ns, s, ac, at);
}

static fts_status_t
mroute_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int n;
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(mroute_t), 1, ac+1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, mroute_init);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, mroute_delete, 0, 0);

  fts_method_define(cl, 0, fts_s_bang, mroute_realize, 0, 0);

  a[0] = fts_s_number;
  fts_method_define(cl, 0, fts_s_int, mroute_realize, 1, a);
  fts_method_define(cl, 0, fts_s_float, mroute_realize, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_list, mroute_realize);

  fts_method_define_varargs(cl, 0, fts_s_anything, mroute_anything);

  return fts_Success;
}

static fts_status_t
iroute_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(iroute_t), 1, ac+1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, iroute_init);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, iroute_delete, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, iroute_int, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, iroute_int, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_list, iroute_list);

  return fts_Success;
}

static fts_status_t
route_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int n;
  const fts_atom_t *tat;

  sym_tilde = fts_new_symbol("~");

  ac--;at++;			/* throw away the class name argument */

  if (!ac)
    {
      post( "route: missing arguments \n");
      return &fts_ArgumentTypeMismatch;
    }

  if (!fts_is_long(at) && !fts_is_symbol(at))
    {
      post( "route: invalid type for first argument\n");
      return &fts_ArgumentTypeMismatch;
    }

  for (n = 1, tat = at+1; n < ac; n++, tat++)
    if (!fts_same_types(at, tat))
      {
	post( "route: argument types don't match\n");
	return &fts_ArgumentTypeMismatch;
      }
	
  return (fts_is_symbol(at) ? mroute_instantiate(cl, ac, at) :
	  iroute_instantiate(cl, ac, at));
}

void
route_config(void)
{
  fts_metaclass_install(fts_new_symbol("route"), route_instantiate, fts_arg_type_equiv);
}

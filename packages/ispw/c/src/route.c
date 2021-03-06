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

typedef struct 
{
  fts_object_t o;
  int ns;
  int *ints;
  fts_symbol_t *symbols;
} route_t;

static void
route_varargs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  route_t *self = (route_t *)o;

  if(self->ints != NULL)
  {
    /* int route */
    int ns = self->ns;
    int n = fts_get_int_arg(ac, at, 0, 0);
    int match = 0;
    int i;
      
    for(i=ns-1; i>=0; i--)
    {
      if (self->ints[i] == n)
      {
	match = 1;
	      
	if(ac == 1)
	  fts_outlet_bang(o, i);
	else if (fts_is_symbol(at))
	  fts_outlet_send(o, i, fts_get_symbol(at), ac - 2, at + 2);
	else 
	  fts_outlet_varargs(o, i, ac - 1, at + 1);
      }
    }
      
    if (!match)
      fts_outlet_varargs(o, ns, ac, at);
  }
  else
  {
    /* route single value or list */
    int ns = self->ns;
    int match = 0;
    int i;

    if(ac == 1)
    {
      /* route single value regarding its selector */
      for(i=ns-1; i>=0; i--)
      {
	if (self->symbols[i] == fts_get_class_name(at))
	{
	  match = 1;
	  fts_outlet_atom(o, i, at);
	}
      }

      if (!match)
	fts_outlet_atom(o, ns, at);
    }
    else if(ac > 0)
    {
      /* route "list" at "list" selector */
      for(i=ns-1; i>=0; i--)
      {
	if (self->symbols[i] == fts_s_list)
	{
	  match = 1;
	  fts_outlet_varargs(o, i, ac, at);
	}
      }
	  
      if (!match)
	fts_outlet_varargs(o, ns, ac, at);
    }
  }
}

static void
route_message(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  route_t *self = (route_t *)o;

  if(self->symbols != NULL)
  {
    int ns = self->ns;
    int match = 0;
    int i;

    /* message */
    for(i=ns-1; i>=0; i--)
    {
      if (self->symbols[i] == s)
      {
	match = 1;
	      
	if (ac == 0)
	  fts_outlet_bang(o, i);
	else if (fts_is_symbol(at))
	  fts_outlet_send(o, i, fts_get_symbol(at), ac - 1, at + 1);
	else
	  fts_outlet_varargs(o, i, ac, at);
      }
    }
      
    if (!match)
      fts_outlet_send(o, ns, s, ac, at);
  }
  else
    fts_object_error(o, "don't understand %s", s);
}

static void
route_bang(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  route_t* self = (route_t*)o;
  int ns = self->ns;
  int match = 0;
  int i;

  if (NULL != self->symbols)
  {
    for (i = ns - 1; i >= 0; --i)
    {
      if (self->symbols[i] == fts_s_bang)
      {
	match = 1;
	fts_outlet_bang(o, i);
      }
    }
  }
  if (0 == match)
  {
    fts_outlet_bang(o, ns);
  }
}

static void
route_input(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if(s != NULL)
  {
    route_message(o, s, ac, at, fts_nix);
  }
  else
  {
    if (0 == ac)
    {
      route_bang(o, NULL, 0, at, fts_nix);
    }
    else
    {
      route_varargs(o, NULL, ac, at, fts_nix);    
    }
  }
}

static void
route_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  route_t *self = (route_t *)o;

  if(ac > 0)
  {
    int all_int = 1;
    int all_symbol = 1;
    int i;
      
    for(i=0; i<ac; i++)
    {
      if(!fts_is_int(at + i))
	all_int = 0;
	  
      if(!fts_is_symbol(at + i))
	all_symbol = 0;
    }
      
    if(all_int)
    {	  
      self->ns = ac;
      self->ints = (int *)fts_malloc(sizeof(int) * ac);
      self->symbols = NULL;
	  
      for(i=0; i<ac; i++)
	self->ints[i] = fts_get_int(at + i);

      fts_object_set_outlets_number(o, ac + 1);
    }
    else if(all_symbol)
    {
      self->ns = ac;
      self->ints = NULL;
      self->symbols = (fts_symbol_t *)fts_malloc(sizeof(fts_symbol_t) * ac);
	  
      for(i=0; i<ac; i++)
	self->symbols[i] = fts_get_symbol(at + i);
	  
      fts_object_set_outlets_number(o, ac + 1);
    }
    else
      fts_object_error(o, "bad arguments");
  }
  else
    fts_object_error(o, "arguments missing");
}

static void
route_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  route_t *self = (route_t *)o;

  if(self->ints != NULL)
    fts_free(self->ints);
  else if(self->symbols != NULL)
    fts_free((void*)self->symbols);
}

static void
route_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(route_t), route_init, route_delete);

  fts_class_input_handler(cl, route_input);

  fts_class_outlet_thru(cl, 0);
  fts_class_outlet_varargs(cl, 0);
}

void
route_config(void)
{
  fts_class_install(fts_new_symbol("route"), route_instantiate);
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */

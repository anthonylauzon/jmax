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
  int ns;
  int *ints;
  fts_symbol_t *symbols;
} route_t;

static void
route_varargs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  route_t *this = (route_t *)o;

  if(this->ints != NULL)
    {
      /* int route */
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
		fts_outlet_varargs(o, i, ac - 1, at + 1);
	    }
	}
      
      if (!match)
	fts_outlet_varargs(o, ns, ac, at);
    }
  else
    {
      /* route single value or list */
      int ns = this->ns;
      int match = 0;
      int i;

      if(ac == 1)
	{
	  /* route single value regarding its selector */
	  for(i=ns-1; i>=0; i--)
	    {
	      if (this->symbols[i] == fts_get_class_name(at))
		{
		  match = 1;
		  fts_outlet_varargs(o, i, 1, at);
		}
	    }

	  if (!match)
	    fts_outlet_varargs(o, ns, 1, at);
	}
      else if(ac > 0)
	{
	  /* route "list" at "list" selector */
	  for(i=ns-1; i>=0; i--)
	    {
	      if (this->symbols[i] == fts_s_list)
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
route_message(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  route_t *this = (route_t *)o;

  if(this->symbols != NULL)
    {
      int ns = this->ns;
      int match = 0;
      int i;

      /* message */
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
		fts_outlet_varargs(o, i, ac, at);
	    }
	}
      
      if (!match)
	fts_outlet_send(o, ns, s, ac, at);
    }
  else
    fts_object_signal_runtime_error(o, "Don't understand %s", s);
}

static void
route_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  route_t *this = (route_t *)o;

  if(ac > 0)
    {
      int all_int = 1;
      int all_symbol = 1;
      int i;
      
      for(i=0; i<ac; i++)
	{
	  if(!fts_is_int(at + 1))
	    all_int = 0;
	  
	  if(!fts_is_symbol(at + 1))
	    all_symbol = 0;
	}
      
      if(all_int)
	{	  
	  this->ns = ac;
	  this->ints = (int *)fts_malloc(sizeof(int) * ac);
	  this->symbols = NULL;
	  
	  for(i=0; i<ac; i++)
	    this->ints[i] = fts_get_int(at + i);

	  fts_object_set_outlets_number(o, ac + 1);
	}
      else if(all_symbol)
	{
	  this->ns = ac;
	  this->ints = NULL;
	  this->symbols = (fts_symbol_t *)fts_malloc(sizeof(fts_symbol_t) * ac);
	  
	  for(i=0; i<ac; i++)
	    this->symbols[i] = fts_get_symbol(at + i);
	  
	  fts_object_set_outlets_number(o, ac + 1);
	}
      else
	fts_object_set_error(o, "Bad arguments");
    }
  else
    fts_object_set_error(o, "Arguments missing");
}

static void
route_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  route_t *this = (route_t *)o;

  if(this->ints != NULL)
    fts_free(this->ints);
  else if(this->symbols != NULL)
    fts_free(this->symbols);
}

static void
route_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(route_t), route_init, route_delete);

  fts_class_set_default_handler(cl, route_message);
  fts_class_inlet_varargs(cl, 0, route_varargs);

  fts_class_outlet_varargs(cl, 0);
}

void
route_config(void)
{
  fts_class_install(fts_new_symbol("route"), route_instantiate);
}

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
 * Authors: Francois Dechelle, Norbert Schnell.
 *
 */
#include <fts/fts.h>
#include "message.h"

fts_symbol_t message_symbol = 0;
fts_type_t message_type = 0;
fts_class_t *message_class = 0;

static int 
is_token(fts_symbol_t s)
{
  return ((s == fts_s_dot) ||
	  (s == fts_s_comma) ||
	  (s == fts_s_semi) ||
	  (s == fts_s_column) ||
	  (s == fts_s_double_colon) ||
	  (s == fts_s_quote) ||
	  (s == fts_s_comma));
}

/**********************************************************
 *
 *  dynamic messages
 *
 */

void
message_clear(message_t *mess)
{
  mess->s = 0;

  if(mess->ac)
    {
      int i;

      /* void atoms */
      for(i=0; i<mess->ac; i++)
	fts_atom_void(mess->at + i);

      fts_block_free(mess->at, sizeof(fts_atom_t) * mess->ac);
  
      mess->ac = 0;
      mess->at = 0;
    }
}

void
message_set(message_t *mess, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int i;

  message_clear(mess);

  mess->s = s;

  if(ac > 0)
    {
      mess->at = (fts_atom_t *)fts_block_alloc(sizeof(fts_atom_t) * ac);
      mess->ac = ac;

      for(i=0; i<ac; i++)
	fts_atom_assign(mess->at + i, at + i);
    }
  else
    {
      mess->at = 0;
      mess->ac = 0;
    }
}

/***************************************************************
 *
 *  class
 *
 */

static void
message_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  message_t *this = (message_t *)o;

  ac--;
  at++;

  this->s = 0;
  this->ac = 0;
  this->at = 0;

  /* check arguments */
  if(ac > 0)
    {
      int i;

      /* check for separators */
      for(i=0; i<ac; i++)
	{
	  fts_symbol_t s = fts_get_symbol(at + i);

	  if(is_token(s))
	    {
	      fts_object_set_error(o, "Syntax error in message or constant");
	      return;
	    }
	}

      if(ac == 1)
	{
	  if(fts_is_list(at))
	    {
	      fts_list_t *aa = fts_get_list(at);
	      message_set(this, fts_s_list, fts_list_get_size(aa), fts_list_get_ptr(aa)); /* explicit list format: "{" [<value> ...] "}" */
	    }
	  else if(!fts_is_symbol(at))
	    message_set(this, fts_get_selector(at), 1, at); /* value format: <non symbol value> (without type specifyer) */
	}
      else if(!fts_is_symbol(at))
	message_set(this, fts_s_list, ac, at); /* implicit list format: <non symbol value> [<value> ...] */
      
      /* first arg is symbol */
      if(fts_is_symbol(at))
	{
	  fts_symbol_t name = fts_get_symbol(at);
	  fts_class_t *cl;

	  if(name == fts_s_list)
	    message_set(this, name, 1, at);
	  else if(fts_atom_type_lookup(name, &cl))
	    {
	      /* constructor format: <class name> [<value> ...] (construct object or primitive type) */
	      
	      /* skip class name */
	      ac--;
	      at++;

	      if(cl)
		{
		  fts_object_t *obj = fts_object_create(cl, ac, at);
		  fts_symbol_t error = fts_object_get_error(obj);
		  
		  if(!error)
		    {
		      fts_atom_t a;
		      
		      fts_set_object_with_type(&a, obj, name);
		      message_set(this, name, 1, &a);
		    }
		  else
		    {
		      fts_object_set_error(o, "%s", fts_symbol_name(error));
		      return;
		    }
		}
	      else if(ac == 1 && fts_get_selector(at) == name)
		message_set(this, name, 1, at);
	      else
		fts_object_set_error(o, "Wrong arguments for %s constructor", fts_symbol_name(name));
	    }
	  else
	    {
	      /* message format: <selector> [<value> ...] (any message - type specifyers are not allowed as selectors) */
	      message_set(this, name, ac - 1, at + 1);
	    }
	}
    }
}

static void
message_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  message_t *this = (message_t *)o;

  message_clear(this);
}

static fts_status_t
message_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(message_t), 0, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, message_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, message_delete);
  
  return fts_Success;
}

void
message_config(void)
{
  message_symbol = fts_new_symbol("message");
  message_type = message_symbol;

  fts_class_install(message_symbol, message_instantiate);
  message_class = fts_class_get_by_name(message_symbol);
}

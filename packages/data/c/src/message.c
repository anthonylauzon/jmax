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
	  (s == fts_s_colon) ||
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

      fts_free(mess->at);
  
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
      mess->at = (fts_atom_t *)fts_malloc(sizeof(fts_atom_t) * ac);
      mess->ac = ac;

      for(i=0; i<ac; i++)
	{
	  fts_set_void(mess->at + i);
	  fts_atom_assign(mess->at + i, at + i);
	}
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

      /* check for separators and lists */
      for(i=0; i<ac; i++)
	{
	  if(fts_is_symbol(at + i) && is_token(fts_get_symbol(at + i)))
	    {
	      fts_object_set_error(o, "Syntax error in message or constant");
	      return;
	    }
	  else if(fts_is_array(at + i))
	    {
	      fts_object_set_error(o, "List cannot be argument of a message or constructor");
	      return;
	    }
	}

      /* first arg is symbol */
      if(fts_is_symbol(at))
	{
	  fts_symbol_t name = fts_get_symbol(at);
	  fts_class_t *cl;

	  if( fts_metaclass_get_by_name( name) != NULL)
	    fts_object_set_error(o, "Symbol %s cannot be used as message", fts_symbol_name(name));
	  else
	    message_set(this, name, ac - 1, at + 1); /* message format: <selector> [<value> ...] (any message) */
	}
      else if(ac == 1)
	message_set(this, fts_get_selector(at), 1, at); /* value format: <non symbol value> (without type specifyer) */
      else
	message_set(this, fts_s_list, ac, at); /* implicit list format: <non symbol value> [<value> ...] */
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

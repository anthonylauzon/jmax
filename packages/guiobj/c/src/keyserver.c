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
 */

#include <fts/fts.h>
#include "keyserver.h"

fts_symbol_t sym_f1 = 0;
fts_symbol_t sym_F1 = 0;
fts_symbol_t sym_f2 = 0;
fts_symbol_t sym_F2 = 0;
fts_symbol_t sym_f3 = 0;
fts_symbol_t sym_F3 = 0;
fts_symbol_t sym_f4 = 0;
fts_symbol_t sym_F4 = 0;
fts_symbol_t sym_f5 = 0;
fts_symbol_t sym_F5 = 0;
fts_symbol_t sym_f6 = 0;
fts_symbol_t sym_F6 = 0;
fts_symbol_t sym_f7 = 0;
fts_symbol_t sym_F7 = 0;
fts_symbol_t sym_f8 = 0;
fts_symbol_t sym_F8 = 0;
fts_symbol_t sym_f9 = 0;
fts_symbol_t sym_F9 = 0;
fts_symbol_t sym_f10 = 0;
fts_symbol_t sym_F10 = 0;
fts_symbol_t sym_f11 = 0;
fts_symbol_t sym_F11 = 0;
fts_symbol_t sym_f12 = 0;
fts_symbol_t sym_F12 = 0;

fts_symbol_t sym_enter = 0;
fts_symbol_t sym_esc = 0;
fts_symbol_t sym_escape = 0;

fts_symbol_t sym_shift = 0;
fts_symbol_t sym_alt = 0;
fts_symbol_t sym_ctrl = 0;

fts_symbol_t sym_space = 0;

listener_t *key_listeners[n_codes];


static void
keyserver_key(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int code = fts_get_int(at);
  listener_t *listener;

  if(code > 0 && code < n_codes)
    {
      listener = key_listeners[code];
      while(listener)
	{
	  listener->meth(listener->obj, 0, 0, ac, at);
	  listener = listener->next;
	}
      
      if(code < 128)
	{
	  listener = key_listeners[0];
	  while(listener)
	    {
	      listener->meth(listener->obj, 0, 0, ac, at);
	      listener = listener->next;
	    }
	}
    }
}

static void 
keyserver_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_object_t), 0, 0);

  fts_class_message_varargs(cl, fts_new_symbol("key"), keyserver_key);
}

void 
keyserver_add_listener(int code, fts_object_t *obj, fts_method_t meth)
{
  if(code >= 0 && code < n_codes)
    {
      listener_t *new = (listener_t *)fts_malloc(sizeof(listener_t));
      
      new->obj = obj;
      new->meth = meth;
      new->next = key_listeners[code];
      
      key_listeners[code] = new;
    }
}

void 
keyserver_remove_listener(int code, fts_object_t *obj)
{
  if(code >= 0 && code < n_codes)
    {
      listener_t *freeme =  0;
      
      if(key_listeners[code] && key_listeners[code]->obj == obj)
	{
	  freeme = key_listeners[code] ;
	  key_listeners[code] = freeme->next;
	}
      else
	{
	  listener_t *l = key_listeners[code];
	  
	  while(l->next)
	    {
	      if(l->next->obj == obj)
		{
		  freeme = l->next;
		  l->next = l->next->next;
		  
		  break;
		}
	      
	      l = l->next;
	    }
	}
      
      /* free removed listener */
      if(freeme)
	fts_free(freeme);
    }
}

void keyserver_config(void)
{
  fts_class_install(fts_new_symbol("_keyserver"), keyserver_instantiate);
}

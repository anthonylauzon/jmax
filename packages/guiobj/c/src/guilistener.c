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
#include <ftsprivate/patcher.h>

typedef struct 
{
  fts_object_t o;
  fts_object_t *source;
} guilistener_t;

static void
guilistener_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_send( o, 0, 0, ac, at);
}

/************************************************************
 *
 *  class
 *
 */

static void
guilistener_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  guilistener_t *this = (guilistener_t *)o;

  this->source = 0;

  if(ac > 0 && fts_is_object(at))
    {
      fts_atom_t a;
      this->source = fts_get_object(at);
      fts_object_refer(this->source);
    
      /* set this as listener of object */
      fts_set_object(&a, o);
      fts_send_message_varargs(this->source, fts_s_add_gui_listener, 1, &a);    
    }
  else
    fts_object_error(o, "valid argument required");
}

static void 
guilistener_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  guilistener_t *this = (guilistener_t *)o;

  if(this->source)
  {
    fts_atom_t a;

    /* remove this as guilistener of source */
    fts_set_object(&a, o);
    fts_send_message_varargs(this->source, fts_s_remove_gui_listener, 1, &a);
    
    fts_object_release(this->source);    
  }
}


static void
guilistener_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(guilistener_t), guilistener_init, guilistener_delete);
  
  fts_class_message_varargs(cl, fts_s_send, guilistener_output);
  
  fts_class_outlet_varargs(cl, 0);
}


void
guilistener_config(void)
{
  fts_class_install(fts_new_symbol("guilistener"), guilistener_instantiate);
}

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
#include <fts/fts.h>
#include "seqsym.h"
#include "event.h"

fts_class_t *event_class = 0;

/**************************************************************
 *
 *  generic event functions
 *
 */

void 
event_get_array(event_t *event, fts_array_t *array)
{
  if(fts_is_object(&event->value))
    {
      fts_object_t *obj = (fts_object_t *)fts_get_object(&event->value);
      fts_symbol_t type = fts_object_get_class_name(obj);
      fts_atom_t a[2];
      
      fts_set_array(a, array);
      fts_send_message(obj, fts_SystemInlet, fts_s_get_array, 1, a);

      fts_set_float(a + 0, (float) event->time);
      fts_set_symbol(a + 1, type);
      fts_array_prepend(array, 2, a);
    }
  else if(!fts_is_void(&event->value))
    {
      fts_symbol_t type = fts_type_get_selector(fts_get_type(&event->value));

      fts_array_append_float(array, (float)event->time);
      fts_array_append_symbol(array, type);
      fts_array_append(array, 1, &event->value);
    }
}

/**************************************************************
 *
 *  generic event methods
 *
 */
static void
event_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  event_t *this = (event_t *)o;

  if(fts_is_object(&this->value))
    {
      fts_object_t *obj = (fts_object_t *)fts_get_object(&this->value);

      fts_send_message(obj, fts_SystemInlet, fts_s_set_from_array, ac, at);
    }
  else
    this->value = at[0];
}

/**************************************************************
 *
 *  class
 *
 */
static void
event_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  event_t *this = (event_t *)o;  

  this->value = at[1];
  this->time = 0.0;
  this->track = 0;
  this->prev = 0;
  this->next = 0;
}

static void
event_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  event_t *this = (event_t *)o;  

  fts_atom_void(&this->value);
}

static fts_status_t
event_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(event_t), 0, 0, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, event_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, event_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, event_set);

  return fts_Success;
}

void
event_config(void)
{
  fts_class_install(seqsym_event, event_instantiate);
  event_class = fts_class_get_by_name(seqsym_event);
}

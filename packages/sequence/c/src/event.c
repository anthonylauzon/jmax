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
#include <fts/packages/sequence/event.h>
#include <fts/packages/sequence/note.h>
#include <fts/packages/sequence/seqsym.h>
#include "seqmess.h"

fts_class_t *event_class = 0;

/**************************************************************
*
*  generic event functions
*
*/

double
event_get_duration(event_t *event)
{
  fts_atom_t *value = &event->value;
  double duration = 0.0;

  if(fts_is_a(value, scoob_class))
  {
    scoob_t *scoob = (scoob_t *)fts_get_object(value);

    duration = scoob_get_duration(scoob);
  }
  else if(fts_is_object(value))
  {
    /* get duration property ?? */
  }

  return duration;
}

/**************************************************************
*
*  generic event methods
*
*/

static fts_array_t event_client_array;

void 
event_set_at_client(event_t *this)
{
  if(fts_is_object(&this->value))
  {
    fts_object_t *obj = fts_get_object(&this->value);
    fts_method_t method_append_properties = fts_class_get_method_varargs(fts_object_get_class(obj), seqsym_append_properties);
    int size = 0;
    fts_atom_t *atoms = 0;

    fts_array_set_size(&event_client_array, 0);

    /* get array of properties and types from class */
    if(method_append_properties)
    {
      fts_atom_t a;

      fts_set_pointer(&a, &event_client_array);

      (*method_append_properties)(obj, 0, 0, 1, &a);

      size = fts_array_get_size(&event_client_array);
      atoms = fts_array_get_atoms(&event_client_array);
    }

    /* send properties to client */
    if(size > 0)
      fts_client_send_message((fts_object_t *)this, fts_s_set, size, atoms);
  }
  else
    fts_client_send_message((fts_object_t *)this, fts_s_set, 1, &this->value);

  fts_object_set_state_dirty((fts_object_t *)event_get_track(this));
}

static void
event_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  event_t *this = (event_t *)o;

  if(fts_is_object(&this->value))
  {
    fts_object_t *obj = (fts_object_t *)fts_get_object(&this->value);
    int i;

    for(i=0; i<ac; i+=2)
    {
      if(ac > i + 1 && fts_is_symbol(at + i))
      {
        fts_symbol_t property = fts_get_symbol(at + i);

        fts_send_message(obj, property, 1, at + i + 1);
      }
    }
  }
  else if(ac > 1 && fts_is_symbol(at) && fts_atom_same_type(&this->value, at+1))
    this->value = at[1];

  event_set_at_client(this);
}

static void
event_unset(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  event_t *this = (event_t *)o;

  if(fts_is_object(&this->value))
  {
    fts_object_t *obj = (fts_object_t *)fts_get_object(&this->value);
    int i;

    for(i=0; i<ac; i++)
    {
      if(fts_is_symbol(at + i))
        fts_send_message(obj, fts_s_remove, 1, at + i);
    }
  }

  event_set_at_client(this);
}

/**************************************************************
*
*  event class
*
*/
static void
event_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  event_t *this = (event_t *)o;

  fts_set_void(&this->value);

  this->time = 0.0;
  this->track = 0;
  this->prev = 0;
  this->next = 0;

  if(ac > 0)
    fts_atom_assign(&this->value, at);
}

static void
event_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  event_t *this = (event_t *)o;

  fts_atom_void(&this->value);
}

static void
event_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(event_t), event_init, event_delete);

  fts_class_message_varargs(cl, fts_s_set, event_set);
  fts_class_message_varargs(cl, fts_new_symbol("unset"), event_unset);
}

/*****************************************************************
*
*  config & install
*
*/

void
event_config(void)
{
  event_class = fts_class_install(seqsym_event, event_instantiate);
  fts_array_init(&event_client_array, 0, 0);
}

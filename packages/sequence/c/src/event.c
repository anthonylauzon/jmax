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
    fts_atom_t ret;
    
    fts_set_void(&ret);
    fts_send_message((fts_object_t *)event, seqsym_duration, 0, NULL, &ret);
    
    if(fts_is_number(&ret))
      duration = fts_get_number_float(&ret);
  }
  
  return duration;
}

void
event_set_duration(event_t *event, double duration)
{
  fts_atom_t *value = &event->value;
  
  if(fts_is_a(value, scoob_class))
  {
    scoob_t *scoob = (scoob_t *)fts_get_object(value);
    
    scoob_set_duration(scoob, duration);
  }
  else if(fts_is_object(value))
  {
    fts_atom_t a;
    
    fts_set_float(&a, duration);
    fts_send_message((fts_object_t *)event, seqsym_duration, 1, &a, fts_nix);
  }
}

void event_unset_property(event_t *event, fts_symbol_t prop)
{
  fts_atom_t *value = &event->value;
  if(fts_is_object(value))
  {
    fts_atom_t a;
  
    fts_set_symbol(&a, prop);
    propobj_remove_property(fts_get_object(value), NULL, 1, &a, fts_nix);
  
    /* poi mandare unset al cliente */
    fts_client_send_message((fts_object_t *)event, fts_s_unset, 1, &a);
  }
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
      
      (*method_append_properties)(obj, NULL, 1, &a, fts_nix);
    }
    else /* not a score object but other object as fmat */ 
    {
      /* register value and send object id as value-property */
      fts_object_t *valobj = fts_get_object( event_get_value( this));
      
      if(fts_object_has_client(valobj) == 0)
        fts_client_register_object(valobj, fts_object_get_client_id((fts_object_t *)this));	
      
      fts_array_append_symbol(&event_client_array, seqsym_objid);
      fts_array_append_int(&event_client_array, fts_object_get_id(valobj));                  
    }    
    
    size = fts_array_get_size(&event_client_array);
    atoms = fts_array_get_atoms(&event_client_array);
    
    /* send properties to client */
    if(size > 0)
      fts_client_send_message((fts_object_t *)this, fts_s_set, size, atoms);
  }
  else
    fts_client_send_message((fts_object_t *)this, fts_s_set, 1, &this->value);
  
  fts_object_set_state_dirty((fts_object_t *)event_get_track(this));
}

static fts_method_status_t
event_set(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
        fts_send_message(obj, property, 1, at + i + 1, fts_nix);
      }
    }
  }
  else if(ac > 1 && fts_is_symbol(at) && fts_atom_same_type(&this->value, at+1))
    this->value = at[1];
  
  event_set_at_client(this);
  
  return fts_ok;
}

static fts_method_status_t
event_unset(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  event_t *this = (event_t *)o;
  
  if(fts_is_object(&this->value))
  {
    fts_object_t *obj = (fts_object_t *)fts_get_object(&this->value);
    int i;
    
    for(i=0; i<ac; i++)
    {
      if(fts_is_symbol(at + i))
        fts_send_message(obj, fts_s_remove, 1, at + i, fts_nix);
    }
  }
  
  event_set_at_client(this);
  
  return fts_ok;
}

/**************************************************************
*
*  event class
*
*/
static fts_method_status_t
event_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  event_t *this = (event_t *)o;
  
  fts_set_void(&this->value);
  
  this->time = 0.0;
  this->track = 0;
  this->prev = 0;
  this->next = 0;
  
  if(ac > 0)
    fts_atom_assign(&this->value, at);
  
  return fts_ok;
}

static fts_method_status_t
event_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  event_t *this = (event_t *)o;
  
  fts_atom_void(&this->value);
  
  return fts_ok;
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

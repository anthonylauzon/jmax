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
#include "seqsym.h"
#include "event.h"

fts_class_t *event_class = 0;

/**************************************************************
 *
 *  generic event functions
 *
 */

fts_symbol_t
event_get_type(event_t *event)
{
  fts_symbol_t type = 0;

  if(fts_is_object(&event->value))
    {
      fts_object_t *obj = (fts_object_t *)fts_get_object(&event->value);
      
      type = fts_object_get_class_name(obj);
    }
  else if(!fts_is_void(&event->value))
    type = fts_type_get_selector(fts_get_type(&event->value));

  return type;
}

void 
event_get_atoms(event_t *event, int *n_atoms, fts_atom_t *atoms)
{
  *n_atoms = 0;

  if(fts_is_object(&event->value))
    {
      fts_object_t *obj = (fts_object_t *)fts_get_object(&event->value);
      fts_atom_t a[2];
      
      fts_set_ptr(a, n_atoms);
      fts_set_ptr(a + 1, atoms);
      
      fts_send_message(obj, fts_SystemInlet, seqsym_get_atoms, 2, a);
    }
  else if(!fts_is_void(&event->value))
    {
      *n_atoms = 1;
      *atoms = event->value;
    }
}

void
event_print(event_t *event)
{
  fts_symbol_t type = event_get_type(event);

  if(type)
    {
      fts_atom_t atoms[64];
      int n_atoms;

      event_get_atoms(event, &n_atoms, atoms);
      
      post("  @%lf: ", event_get_time(event));
      post("<%s> ", fts_symbol_name(type));
      post_atoms(n_atoms, atoms);
      post("\n");
    }
}

void
event_upload(event_t *event)
{
  fts_symbol_t type = event_get_type(event);

  if(type)
    {
      fts_atom_t atoms[64];
      int n_atoms;

      fts_set_float(atoms + 0, event->time);
      fts_set_symbol(atoms + 1, type);
  
      event_get_atoms(event, &n_atoms, atoms + 2);
  
      fts_client_upload((fts_object_t *)event, seqsym_event, n_atoms + 2, atoms);
    }
}

void
event_save_bmax(fts_bmax_file_t *file, event_t *event)
{
  fts_symbol_t type = event_get_type(event);

  if(type)
    {
      fts_atom_t atoms[64];
      int n_atoms;

      fts_set_float(atoms + 0, event->time);
      fts_set_symbol(atoms + 1, type);
  
      event_get_atoms(event, &n_atoms, atoms + 2);
  
      fts_bmax_save_message(file, seqsym_bmax_add_event, n_atoms + 2, atoms);
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

      fts_send_message(obj, fts_SystemInlet, seqsym_set, ac, at);
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

  if(fts_is_object(&this->value))
    {
      fts_object_t *obj = (fts_object_t *)fts_get_object(&this->value);
      fts_object_delete(obj);
    }
}

static fts_status_t
event_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(event_t), 0, 0, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, event_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, event_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_set, event_set);

  return fts_Success;
}

void
event_config(void)
{
  fts_class_install(seqsym_event, event_instantiate);
  event_class = fts_class_get_by_name(seqsym_event);
}



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
#include "bpf.h"
#include "ivec.h"
#include "fvec.h"

/******************************************************
 *
 *  preset
 *
 */

static fts_symbol_t sym_get_state_as_array = 0;
static fts_symbol_t sym_restore_state_from_array = 0;

typedef struct 
{
  fts_object_t o;
  fts_object_t **objects;
  fts_list_t *states;
  int n_objects;
  int n_presets;
} preset_t;

static int
preset_check_object(preset_t *this, fts_object_t *obj)
{
  fts_class_t *class = fts_object_get_class(obj);
  fts_method_t get_meth = fts_class_get_method(class, fts_SystemInlet, sym_get_state_as_array);
  fts_method_t restore_meth = fts_class_get_method(class, fts_SystemInlet, sym_restore_state_from_array);

  return (get_meth != 0 && restore_meth != 0);
}

static void
preset_clear(preset_t *this, int n)
{
  int n_objects = this->n_objects;
  fts_list_t *states = this->states + n_objects * n;
  int i;

  for(i=0; i<n_objects; i++)
    fts_list_reset(states + i);
}

static void
preset_clear_all(preset_t *this)
{
  int n_presets = this->n_presets;
  int i;

  for(i=0; i<n_presets; i++)
    preset_clear(this, i);
}

/******************************************************
 *
 *  methods
 *
 */

static void
preset_store(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preset_t *this = (preset_t *)o;

  if(fts_is_number(at))
    {
      int n = fts_get_number_int(at);

      if(n >= 0 && n < this->n_presets)
	{
	  int n_objects = this->n_objects;
	  fts_list_t *states = this->states + n_objects * n;
	  fts_atom_t a;
	  int i;

	  for(i=0; i<n_objects; i++)
	    {
	      /* clear eventual previous content */
	      fts_list_reset(states + i);

	      /* get object state as array */
	      fts_set_list(&a, states + i);
	      fts_message_send(this->objects[i], fts_SystemInlet, sym_get_state_as_array, 1, &a);
	    }
	}
    }
}

static void
preset_recall(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preset_t *this = (preset_t *)o;

  if(fts_is_number(at))
    {
      int n = fts_get_number_int(at);

      if(n >= 0 && n < this->n_presets)
	{
	  int n_objects = this->n_objects;
	  fts_list_t *states = this->states + n_objects * n;
	  fts_atom_t a;
	  int i;

	  for(i=0; i<n_objects; i++)
	    {
	      fts_list_t *atoms = states + i;
	      fts_atom_t *ptr = fts_list_get_ptr(states + i);
	      int size = fts_list_get_size(states + i);

	      if(size)
		fts_message_send(this->objects[i], fts_SystemInlet, sym_restore_state_from_array, size, ptr);
	    }
	}
    }
}

/******************************************************
 *
 *  class
 *
 */

static void
preset_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preset_t *this = (preset_t *)o;
  int n_objects = 0;
  int n_presets = 0;
  int i;

  ac--;
  at++;

  if(fts_is_number(at))
    {
      n_presets = fts_get_number_int(at);

      ac--;
      at++;
    }

  if(n_presets <= 0)
    n_presets = 16;

  n_objects = ac;

  for(i=0; i<n_objects; i++)
    {
      if(fts_is_object(at))
	{
	  fts_object_t *obj = fts_get_object(at + i);
	  
	  if(!preset_check_object(this, obj))
	    {
	      fts_symbol_t name = fts_object_get_class_name(obj);
	      fts_object_set_error(o, "Cannot handle object of class %s", fts_symbol_name(name));
	      return;
	    }
	}
      else
	{
	  fts_object_set_error(o, "Wrong arguments");
	  return;
	}
    }

  this->objects = (fts_object_t **)fts_malloc(sizeof(fts_object_t *) * n_objects);

  for(i=0; i<n_objects; i++)
    {
      fts_object_t *obj = fts_get_object(at + i);
      this->objects[i] = obj;
      fts_object_refer(obj);
    }

  this->states = (fts_list_t *)fts_malloc(sizeof(fts_list_t) * n_objects * n_presets);

  for(i=0; i<n_objects * n_presets; i++)
    fts_list_init(this->states + i, 0, 0);

  this->n_objects = n_objects;
  this->n_presets = n_presets;
}

static void
preset_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preset_t *this = (preset_t *)o;
  int i;

  preset_clear_all(this);

  fts_free(this->states);

  for(i=0; i<this->n_objects; i++)
    fts_object_release(this->objects[i]);
}

static fts_status_t
preset_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(preset_t), 2, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, preset_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, preset_delete);

  fts_method_define_varargs(cl, 0, fts_new_symbol("store"), preset_store);
  fts_method_define_varargs(cl, 0, fts_new_symbol("recall"), preset_recall);

  return fts_Success;
}

void
preset_config(void)
{
  sym_get_state_as_array = fts_new_symbol("get_state_as_array");
  sym_restore_state_from_array = fts_new_symbol("restore_state_from_array");

  fts_class_install(fts_new_symbol("preset"), preset_instantiate);
}

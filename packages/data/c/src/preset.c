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
#include "preset.h"

/******************************************************
 *
 *  preset
 *
 */

fts_symbol_t preset_symbol = 0;
fts_type_t preset_type = 0;
fts_class_t *preset_class = 0;

static fts_symbol_t sym_add_state_from_bmax = 0;
static fts_symbol_t sym_add_array_from_bmax = 0;

static int
preset_check_object(preset_t *this, fts_object_t *obj)
{
  fts_class_t *class = fts_object_get_class(obj);
  fts_method_t get_meth = fts_class_get_method(class, fts_SystemInlet, fts_s_append_state_to_array);
  fts_method_t restore_meth = fts_class_get_method(class, fts_SystemInlet, fts_s_set_state_from_array);

  return (get_meth != 0 && restore_meth != 0);
}

static void
preset_remove(preset_t *this, const fts_atom_t *key)
{
  fts_atom_t value;

  if(fts_hashtable_get(&this->hash, key, &value))
    {
      fts_array_t *states = (fts_array_t *)fts_get_ptr(&value);
      int i;

      for(i=0; i<this->n_objects; i++)
	fts_array_destroy(states + i);

      fts_free(states);
      fts_hashtable_remove(&this->hash, key);      
    }
}

static fts_array_t *
preset_get_or_add(preset_t *this, const fts_atom_t *key)
{
  fts_array_t *states;
  fts_atom_t value;
  
  if(fts_hashtable_get(&this->hash, key, &value))
    states = fts_get_array(&value);
  else
    {
      int i;
      
      states = (fts_array_t *)fts_malloc(sizeof(fts_array_t) * this->n_objects);
      
      for(i=0; i<this->n_objects; i++)
	fts_array_init(states + i, 0, 0);

      fts_set_ptr(&value, (void *)states);
      fts_hashtable_put(&this->hash, key, &value);
    }

  return states;
}

void
preset_get_keys(preset_t *this, fts_array_t *array)
{
  fts_iterator_t iterator;
  
  fts_hashtable_get_keys(&this->hash, &iterator);
  
  while(fts_iterator_has_more( &iterator))
    {
      fts_atom_t key;
      
      fts_iterator_next( &iterator, &key);
      fts_array_append(array, 1, &key);
    }
}

/******************************************************
 *
 *  methods
 *
 */

static void
preset_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preset_t *this = (preset_t *)o;

  if(ac > 0)
    {
      if(fts_is_int(at))
	preset_remove(this, at);
    }
  else
    {
      /* clear all */
      fts_iterator_t iterator;
      
      fts_hashtable_get_keys(&this->hash, &iterator);
      
      while(fts_iterator_has_more( &iterator))
	{
	  fts_atom_t key;
	  fts_array_t *states;
	  int i;
	  
	  fts_iterator_next( &iterator, &key);
	  preset_remove(this, &key);
	}
      
      fts_hashtable_clear(&this->hash);
    }
}

static void
preset_store(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preset_t *this = (preset_t *)o;
  int i;  

  if(fts_is_int(at))
    {
      fts_atom_t value;
      fts_array_t *states;

      if(fts_hashtable_get(&this->hash, at, &value))
	{
	  states = (fts_array_t *)fts_get_ptr(&value);
	  
	  for(i=0; i<this->n_objects; i++)
	    {
	      fts_array_set_size(states + i, 0);

	      /* get object state as array */
	      fts_set_array(&value, states + i);
	      fts_message_send(this->objects[i], fts_SystemInlet, fts_s_append_state_to_array, 1, &value);
	    }
	}
      else
	{
	  states = (fts_array_t *)fts_malloc(sizeof(fts_array_t) * this->n_objects);
	  
	  for(i=0; i<this->n_objects; i++)
	    {
	      fts_array_init(states + i, 0, 0);

	      /* get object state as array */
	      fts_set_array(&value, states + i);
	      fts_message_send(this->objects[i], fts_SystemInlet, fts_s_append_state_to_array, 1, &value);
	    }

	  fts_set_ptr(&value, (void *)states);
	  fts_hashtable_put(&this->hash, at, &value);
	}
    }
}

static void
preset_recall(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preset_t *this = (preset_t *)o;

  if(fts_is_int(at))
    {
      fts_atom_t value;

      if(fts_hashtable_get(&this->hash, at, &value))
	{
	  fts_array_t *states = fts_get_array(&value);
	  int i;
	  
	  for(i=0; i<this->n_objects; i++)
	    {
	      fts_atom_t *atoms = fts_array_get_atoms(states + i);
	      int size = fts_array_get_size(states + i);

	      fts_message_send(this->objects[i], fts_SystemInlet, fts_s_set_state_from_array, size, atoms);
	    }

	  fts_outlet_send(o, 0, fts_s_int, 1, at);
	}
    }
}

/******************************************************
 *
 *  bmax files
 *
 */

static void
preset_add_state_from_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preset_t *this = (preset_t *)o;
  fts_array_t *states = preset_get_or_add(this, at + 0);
  int i_object = fts_get_int(at + 1);

  fts_array_set(states + i_object, ac - 2, at + 2);
}

/* add array to preset at given index of state of given object */
static void
preset_add_array_from_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preset_t *this = (preset_t *)o;
  fts_array_t *states = preset_get_or_add(this, at + 0);
  int i_object = fts_get_int(at + 1);
  int i_array = fts_get_int(at + 2);

  if(i_object < this->n_objects)
    {
      fts_array_t *atoms = fts_malloc(sizeof(fts_array_t));
      fts_atom_t *a = fts_array_get_element(states + i_object, i_array);

      fts_array_init(atoms, ac - 3, at + 3);
      fts_set_array(a, atoms);
    }
}

static void
preset_save_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preset_t *this = (preset_t *)o;

  if(this->keep == fts_s_yes)
    {
      fts_bmax_file_t *f = (fts_bmax_file_t *)fts_get_ptr(at);
      fts_iterator_t iterator;
      
      fts_hashtable_get_keys(&this->hash, &iterator);
      
      while(fts_iterator_has_more( &iterator))
	{
	  fts_atom_t key;
	  fts_atom_t value;
	  fts_array_t *states;
	  int i;
	  
	  fts_iterator_next( &iterator, &key);
	  fts_hashtable_get(&this->hash, &key, &value);
	  
	  states = (fts_array_t *)fts_get_ptr(&value);
	      
	  /* save states */
	  for(i=0; i<this->n_objects; i++)
	    {
	      fts_atom_t *atoms = fts_array_get_atoms(states + i);
	      int size = fts_array_get_size(states + i);
	      int j;

	      /* save state as array */
	      for(j=0; j<size; j++)
		{
		  /* if list init state atom as 0 */
		  if(fts_is_list(atoms + j))
		    fts_bmax_code_push_int(f, 0);
		  else
		    fts_bmax_code_push_atoms(f, 1, atoms + j);
		}
	      
	      /* write object index */
	      fts_bmax_code_push_int(f, i);

	      /* write key */
	      fts_bmax_code_push_atoms(f, 1, &key);

	      fts_bmax_code_obj_mess(f, fts_SystemInlet, sym_add_state_from_bmax, size + 2);
	      fts_bmax_code_pop_args(f, size + 2);

	      /* save sub arrays */
	      for(j=0; j<size; j++)
		{
		  if(fts_is_list(atoms + j))
		    {
		      fts_array_t *array = fts_get_array(atoms + j);
		      int n = fts_array_get_size(array);

		      /* write array */
		      fts_bmax_code_push_atoms(f, n, fts_array_get_atoms(array));

		      /* write array index */
		      fts_bmax_code_push_int(f, j);

		      /* write object index */
		      fts_bmax_code_push_int(f, i);
		      
		      /* write key */
		      fts_bmax_code_push_atoms(f, 1, &key);
		      
		      fts_bmax_code_obj_mess(f, fts_SystemInlet, sym_add_array_from_bmax, n + 3);
		      fts_bmax_code_pop_args(f, n + 3);
		    }
		}

	    }
	}
    }
}

static void
preset_set_keep(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  preset_t *this = (preset_t *)obj;

  if(this->keep != fts_s_args && fts_is_symbol(value))
    this->keep = fts_get_symbol(value);
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
  int i;

  ac--;
  at++;

  /* check arguments */
  for(i=0; i<ac; i++)
    {
      if(fts_is_object(at + i))
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
	  fts_object_set_error(o, "Arguments of object required", i);
	  return;
	}
    }

  /* init hash table */
  fts_hashtable_init(&this->hash, FTS_HASHTABLE_INT, FTS_HASHTABLE_SMALL);

  this->objects = (fts_object_t **)fts_malloc(sizeof(fts_object_t *) * ac);

  for(i=0; i<ac; i++)
    {
      fts_object_t *obj = fts_get_object(at + i);
      this->objects[i] = obj;
      fts_object_refer(obj);
    }

  this->n_objects = ac;
  this->keep = fts_s_no;
}

static void
preset_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preset_t *this = (preset_t *)o;
  int i;

  preset_clear(o, 0, 0, 0, 0);

  for(i=0; i<this->n_objects; i++)
    fts_object_release(this->objects[i]);
}

static void
preset_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  preset_t *this = (preset_t *) obj;
  
  fts_set_object_with_type(value, (fts_object_t *)this, preset_symbol);
}

static fts_status_t
preset_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(preset_t), 2, 1, 0);
  
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, preset_get_state);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, preset_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, preset_delete);

  /* save and restore to/from bmax file */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_bmax, preset_save_bmax); 
  fts_method_define_varargs(cl, fts_SystemInlet, sym_add_state_from_bmax, preset_add_state_from_bmax);
  fts_method_define_varargs(cl, fts_SystemInlet, sym_add_array_from_bmax, preset_add_array_from_bmax);

  /* persistency */
  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("keep"), preset_set_keep);

  fts_method_define_varargs(cl, 0, fts_new_symbol("store"), preset_store);
  fts_method_define_varargs(cl, 0, fts_new_symbol("recall"), preset_recall);
  fts_method_define_varargs(cl, 0, fts_new_symbol("clear"), preset_clear);

  return fts_Success;
}

void
preset_config(void)
{
  fts_s_append_state_to_array = fts_s_append_state_to_array;
  fts_s_set_state_from_array = fts_s_set_state_from_array;

  sym_add_state_from_bmax = fts_new_symbol("add_state_from_bmax");
  sym_add_array_from_bmax = fts_new_symbol("add_array_from_bmax");

  preset_symbol = fts_new_symbol("preset");
  preset_type = preset_symbol;

  fts_class_install(preset_symbol, preset_instantiate);
  preset_class = fts_class_get_by_name(preset_symbol);
}


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

static fts_symbol_t sym_preset_dumper = 0;
static fts_symbol_t sym_new_preset = 0;
static fts_symbol_t sym_dump_mess = 0;

/******************************************************
 *
 *  preset dumper utility
 *
 */

static fts_metaclass_t *preset_dumper_type = 0;

typedef struct 
{
  fts_dumper_t head;
  fts_dumper_t *dumper;
  int index;
} preset_dumper_t;

#define preset_dumper_set_index(d, i) ((d)->index = (i))

static void
preset_dumper_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preset_dumper_t *this = (preset_dumper_t *)o;
  fts_message_t *mess;
  
  mess = fts_dumper_message_new(this->dumper, sym_dump_mess);

  fts_message_append_int(mess, this->index);
  fts_message_append_symbol(mess, s);
  fts_message_append(mess, ac, at);

  fts_dumper_message_send(this->dumper, mess);
}

static void
preset_dumper_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preset_dumper_t *this = (preset_dumper_t *)o;

  /* init super */
  fts_dumper_init((fts_dumper_t *)this, preset_dumper_send);

  this->dumper = (fts_dumper_t *)fts_get_object(at);
  fts_object_refer(this->dumper);
}
  
static void
preset_dumper_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preset_dumper_t *this = (preset_dumper_t *)o;
  
  fts_object_release(this->dumper);

  /* delete super */
  fts_dumper_destroy((fts_dumper_t *)this);
}
  
static fts_status_t
preset_dumper_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(preset_dumper_t), 0, 0, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, preset_dumper_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, preset_dumper_delete);

  return fts_ok;
}

/******************************************************
 *
 *  preset
 *
 */

fts_symbol_t preset_symbol = 0;
fts_metaclass_t *preset_type = 0;

static int
preset_check_object(preset_t *this, fts_object_t *obj)
{
  fts_class_t *class = fts_object_get_class(obj);
  fts_method_t meth_set = fts_class_get_method(class, fts_system_inlet, fts_s_set_from_instance);
  fts_method_t meth_dump = fts_class_get_method(class, fts_system_inlet, fts_s_dump);

  return (meth_set != 0) && (meth_dump != 0);
}

static void
preset_remove(preset_t *this, const fts_atom_t *key)
{
  fts_atom_t value;

  if(fts_hashtable_get(&this->hash, key, &value))
    {
      fts_object_t **clones = (fts_object_t **)fts_get_pointer(&value);
      int i;

      for(i=0; i<this->n_objects; i++)
	fts_object_release(clones[i]);

      fts_free(clones);
      fts_hashtable_remove(&this->hash, key);      
    }
}

static fts_object_t **
preset_get_or_add(preset_t *this, const fts_atom_t *key)
{
  fts_atom_t value;
  fts_object_t **clones;

  if(fts_hashtable_get(&this->hash, key, &value))
    {
      int i;

      clones = (fts_object_t **)fts_get_pointer(&value);

      for(i=0; i<this->n_objects; i++)
	{
	  fts_object_release(clones[i]);
	  clones[i] = 0;
	}
    }
  else
    {
      int i;

      clones = (fts_object_t **)fts_malloc(sizeof(fts_object_t *) * this->n_objects);
      
      for(i=0; i<this->n_objects; i++)
	clones[i] = 0;

      fts_set_pointer(&value, (void *)clones);
      fts_hashtable_put(&this->hash, key, &value);
    }

  return clones;
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

  if(fts_is_int(at))
    {
      fts_object_t **objects = this->objects;
      fts_object_t **clones;
      fts_atom_t a;
      int i;  
  
      if(fts_hashtable_get(&this->hash, at, &a))
	{
	  /* overwrite existing preset */
	  clones = (fts_object_t **)fts_get_pointer(&a);
	  
	  for(i=0; i<this->n_objects; i++)
	    {
	      fts_metaclass_t *type = fts_object_get_metaclass(this->objects[i]);

	      /* release old clone */
	      fts_object_release(clones[i]);

	      /* create new clone */
	      clones[i] = fts_object_create(type, 0, 0);

	      fts_set_object(&a, objects[i]);
	      fts_send_message(clones[i], fts_system_inlet, fts_s_set_from_instance, 1, &a);

	      fts_object_refer(clones[i]);
	    }
	}
      else
	{
	  /* create new preset */
	  clones = (fts_object_t **)fts_malloc(sizeof(fts_object_t *) * this->n_objects);
	  
	  for(i=0; i<this->n_objects; i++)
	    {
	      fts_metaclass_t *type = fts_object_get_metaclass(this->objects[i]);

	      /* create new clone */
	      clones[i] = fts_object_create(type, 0, 0);

	      fts_set_object(&a, objects[i]);
	      fts_send_message(clones[i], fts_system_inlet, fts_s_set_from_instance, 1, &a);

	      fts_object_refer(clones[i]);
	    }

	  /* store new preset */
	  fts_set_pointer(&a, (void *)clones);
	  fts_hashtable_put(&this->hash, at, &a);
	}
    }
}

static void
preset_recall(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preset_t *this = (preset_t *)o;

  if(fts_is_int(at))
    {
      int n = fts_get_int(at);
      fts_atom_t a;

      if(fts_hashtable_get(&this->hash, at, &a))
	{
	  fts_object_t **clones = fts_get_pointer(&a);
	  int i, j;
	  
	  for(i=0; i<this->n_objects; i++)
	    {
	      fts_atom_t a;

	      fts_set_object(&a, clones[i]);
	      fts_send_message(this->objects[i], fts_system_inlet, fts_s_set_from_instance, 1, &a);
	    }

	  fts_outlet_int(o, 0, n);
	}
    }
}

static void
preset_new_preset(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preset_t *this = (preset_t *)o;

  this->current = preset_get_or_add(this, at + 0);; 
}

static void
preset_dump_mess(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preset_t *this = (preset_t *)o;
  int index = fts_get_int(at + 0);
  fts_symbol_t selector = fts_get_symbol(at + 1);
  fts_atom_t a;
  
  /* create new clone */
  if(!this->current[index])
    {
      fts_metaclass_t *type = fts_object_get_metaclass(this->objects[index]);

      this->current[index] = fts_object_create(type, 0, 0);
    }

  fts_send_message(this->current[index], fts_system_inlet, selector, ac - 2, at + 2);
}

static void
preset_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preset_t *this = (preset_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  preset_dumper_t *preset_dumper = (preset_dumper_t *)fts_object_create(preset_dumper_type, 1, at);
  fts_iterator_t iterator;

  fts_object_refer(preset_dumper);
  
  /* dump presets */
  fts_hashtable_get_keys(&this->hash, &iterator);      
  
  while(fts_iterator_has_more( &iterator))
    {
      fts_object_t **clones;
      fts_atom_t key;
      fts_atom_t a;
      int i;
      
      /* get preset clones */
      fts_iterator_next( &iterator, &key);
      fts_hashtable_get(&this->hash, &key, &a);
      
      clones = (fts_object_t **)fts_get_pointer(&a);
      
      /* dump preset message */
      fts_dumper_send(dumper, sym_new_preset, 1, &key);

      /* dump messages of clones */
      for(i=0; i<this->n_objects; i++)
	{
	  /* set current clone index */
	  preset_dumper_set_index(preset_dumper, i);
	  
	  /* dump clone messages */
	  fts_set_object(&a, (fts_object_t *)preset_dumper);
	  fts_send_message(clones[i], fts_system_inlet, fts_s_dump, 1, &a);
	}
    }
  
  fts_object_release((fts_object_t *)preset_dumper);
}

static void
preset_get_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preset_t *this = (preset_t *)o;
  fts_array_t *array = (fts_array_t *)fts_get_pointer(at);

  preset_get_keys(this, array);
}

static void
preset_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object(value, obj);
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

  /* check arguments */
  for(i=0; i<ac; i++)
    {
      if(fts_is_object(at + i))
	{
	  fts_object_t *obj = fts_get_object(at + i);
	  
	  if(!preset_check_object(this, obj))
	    {
	      fts_symbol_t name = fts_object_get_class_name(obj);
	      fts_object_set_error(o, "Cannot handle object of class %s", name);
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
  data_object_set_keep((data_object_t *)o, fts_s_no);
  this->current = 0;
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

static fts_status_t
preset_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(preset_t), 1, 1, 0);
  
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, preset_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, preset_delete);

  /* save and restore to/from bmax file */
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_dump, preset_dump); 
  fts_method_define_varargs(cl, fts_system_inlet, sym_new_preset, preset_new_preset);
  fts_method_define_varargs(cl, fts_system_inlet, sym_dump_mess, preset_dump_mess);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_get_array, preset_get_array);

  /* persistency */
  fts_class_add_daemon(cl, obj_property_put, fts_s_keep, data_object_daemon_set_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_keep, data_object_daemon_get_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, preset_get_state);

  fts_method_define_varargs(cl, 0, fts_new_symbol("store"), preset_store);
  fts_method_define_varargs(cl, 0, fts_new_symbol("recall"), preset_recall);
  fts_method_define_varargs(cl, 0, fts_s_clear, preset_clear);

  return fts_ok;
}

void
preset_config(void)
{
  preset_symbol = fts_new_symbol("preset");

  sym_new_preset = fts_new_symbol("new_preset");
  sym_dump_mess = fts_new_symbol("dump_mess");
  sym_preset_dumper = fts_new_symbol("preset_dumper");

  preset_type = fts_class_install(preset_symbol, preset_instantiate);
  preset_dumper_type = fts_class_install(sym_preset_dumper, preset_dumper_instantiate);
}

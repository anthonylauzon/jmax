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

static fts_symbol_t sym_new_preset = 0;
static fts_symbol_t sym_dump_mess = 0;

fts_class_t *fts_preset_class = 0;

/******************************************************
 *
 *  preset dumper utility
 *
 */

static fts_class_t *preset_dumper_type = 0;

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
  
static void
preset_dumper_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(preset_dumper_t), preset_dumper_init, preset_dumper_delete);
}

/******************************************************
 *
 *  preset
 *
 */

static int
preset_check_object(fts_preset_t *this, fts_object_t *obj)
{
  fts_class_t *class = fts_object_get_class(obj);
  fts_method_t meth_set_from_instance = fts_class_get_method(class, fts_s_set_from_instance);
  fts_method_t meth_dump_state = fts_class_get_method(class, fts_s_dump_state);

  return (meth_set_from_instance != 0) && (meth_dump_state != 0);
}

static void
preset_remove(fts_preset_t *this, const fts_atom_t *key)
{
  fts_atom_t value;

  if(fts_hashtable_get(&this->hash, key, &value))
    {
      fts_object_t **clones = (fts_object_t **)fts_get_pointer(&value);
      int i;

      for(i=0; i<this->n_objects; i++)
	{
	  if(clones[i] != NULL)
	    fts_object_release(clones[i]);
	}

      fts_free(clones);
      fts_hashtable_remove(&this->hash, key);      
    }
}

static fts_object_t **
preset_get_or_add(fts_preset_t *this, const fts_atom_t *key)
{
  fts_atom_t value;
  fts_object_t **clones;

  if(fts_hashtable_get(&this->hash, key, &value))
    {
      int i;

      /* empty existing preset */
      clones = (fts_object_t **)fts_get_pointer(&value);

      for(i=0; i<this->n_objects; i++)
	{
	  if(clones[i] != NULL)
	    {
	      fts_object_release(clones[i]);
	      clones[i] = NULL;
	    }
	}
    }
  else
    {
      int i;

      /* create new preset */
      clones = (fts_object_t **)fts_malloc(sizeof(fts_object_t *) * this->n_objects);
      
      for(i=0; i<this->n_objects; i++)
	clones[i] = NULL;

      fts_set_pointer(&value, (void *)clones);
      fts_hashtable_put(&this->hash, key, &value);
    }

  return clones;
}

void
preset_get_keys(fts_preset_t *this, fts_array_t *array)
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
  fts_preset_t *this = (fts_preset_t *)o;

  if(ac > 0)
    {
      if(fts_is_number(at))
	{
	  fts_atom_t a;
	  
	  fts_set_int(&a, fts_get_number_int(at));
	  preset_remove(this, &a);
	}    
    }
  else
    {
      /* clear all */
      fts_iterator_t iterator;
      
      fts_hashtable_get_keys(&this->hash, &iterator);
      
      while(fts_iterator_has_more( &iterator))
	{
	  fts_atom_t key;
	  
	  fts_iterator_next( &iterator, &key);
	  preset_remove(this, &key);
	}
      
      fts_hashtable_clear(&this->hash);
    }
}

static void
preset_store(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_preset_t *this = (fts_preset_t *)o;

  if(fts_is_number(at))
    {
      fts_object_t **objects = this->objects;
      fts_object_t **clones;
      fts_atom_t k, a;
      int i;  

      fts_set_int(&k, fts_get_number_int(at));
  
      if(fts_hashtable_get(&this->hash, &k, &a))
	{
	  /* overwrite existing preset */
	  clones = (fts_object_t **)fts_get_pointer(&a);
	  
	  for(i=0; i<this->n_objects; i++)
	    {
	      fts_class_t *type = fts_object_get_class(this->objects[i]);

	      /* release old clone */
	      if(clones[i] != NULL)
		fts_object_release(clones[i]);

	      /* create new clone */
	      clones[i] = fts_object_create(type, NULL, 0, 0);

	      fts_set_object(&a, objects[i]);
	      fts_send_message(clones[i], fts_s_set_from_instance, 1, &a);

	      fts_object_refer(clones[i]);
	    }
	}
      else
	{
	  /* create new preset */
	  clones = (fts_object_t **)fts_malloc(sizeof(fts_object_t *) * this->n_objects);
	  
	  for(i=0; i<this->n_objects; i++)
	    {
	      fts_class_t *type = fts_object_get_class(this->objects[i]);

	      /* create new clone */
	      clones[i] = fts_object_create(type, NULL, 0, 0);

	      fts_set_object(&a, objects[i]);
	      fts_send_message(clones[i], fts_s_set_from_instance, 1, &a);

	      fts_object_refer(clones[i]);
	    }

	  /* store new preset */
	  fts_set_pointer(&a, (void *)clones);
	  fts_hashtable_put(&this->hash, &k, &a);
	}
    }
}

static void
preset_recall(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_preset_t *this = (fts_preset_t *)o;

  if(fts_is_number(at))
    {
      int n = fts_get_number_int(at);
      fts_atom_t k, a;

      fts_set_int(&k, n);

      if(fts_hashtable_get(&this->hash, &k, &a))
	{
	  fts_object_t **clones = fts_get_pointer(&a);
	  int i;
	  
	  for(i=0; i<this->n_objects; i++)
	    {
	      if(clones[i] != NULL)
		{
		  fts_atom_t a;
		  
		  fts_set_object(&a, clones[i]);
		  fts_send_message(this->objects[i], fts_s_set_from_instance, 1, &a);
		}
	    }

	  fts_outlet_int(o, 0, n);
	}
    }
}

static void
preset_get_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_preset_t *this = (fts_preset_t *)o;
  fts_array_t *array = (fts_array_t *)fts_get_pointer(at);

  preset_get_keys(this, array);
}

static void
preset_new_preset(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_preset_t *this = (fts_preset_t *)o;

  this->current = preset_get_or_add(this, at + 0);
}

static void
preset_dump_mess(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_preset_t *this = (fts_preset_t *)o;
  int index = fts_get_int(at + 0);
  fts_symbol_t selector = fts_get_symbol(at + 1);
  
  /* create new clone */
  if(!this->current[index])
    {
      fts_class_t *type = fts_object_get_class(this->objects[index]);

      this->current[index] = fts_object_create(type, NULL, 0, 0);
    }

  fts_send_message(this->current[index], selector, ac - 2, at + 2);
}

static void
preset_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_preset_t *this = (fts_preset_t *)o;

  if(this->persistence != 0)
    {
      fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
      preset_dumper_t *preset_dumper = (preset_dumper_t *)fts_object_create(preset_dumper_type, NULL, 1, at);
      fts_iterator_t iterator;
      fts_atom_t a;

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
	  
	      if(clones[i] != NULL)
		{
		  /* dump clone messages */
		  fts_set_object(&a, (fts_object_t *)preset_dumper);
		  fts_send_message(clones[i], fts_s_dump_state, 1, &a);
		}
	    }
	}

      fts_object_release((fts_object_t *)preset_dumper);

      /* save persistence flag */  
      fts_set_int(&a, 1);
      fts_dumper_send(dumper, fts_s_persistence, 1, &a);      

      fts_name_dump_method(o, 0, 0, ac, at);
    }
}

static void
preset_persistence(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_preset_t *this = (fts_preset_t *)o;

  if(ac > 0)
    {
      if(fts_is_number(at))
	{
	  this->persistence = (fts_get_number_int(at) != 0);
	  fts_client_send_message(o, fts_s_persistence, 1, at);
	}
    }
  else
    {
      fts_atom_t a;

      fts_set_int(&a, this->persistence);
      fts_return(&a);
    }
}

static void
preset_update_gui(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_preset_t *this = (fts_preset_t *)o;
  fts_atom_t a;    

  if(this->persistence >= 0)
    {
      fts_set_int(&a, this->persistence);
      fts_client_send_message(o, fts_s_persistence, 1, &a);
    }

  fts_name_gui_method(o, 0, 0, 0, 0);
}

static void
preset_redefine(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_preset_t *this = (fts_preset_t *)o;
  fts_preset_t *old = (fts_preset_t *)fts_get_object(at);

  if(fts_object_get_class((fts_object_t *)old) == fts_preset_class)
    {
      int n = this->n_objects;
      fts_iterator_t iterator;
      fts_atom_t a;
      
      if(n > old->n_objects)
	n = old->n_objects;
      
      fts_hashtable_get_keys(&old->hash, &iterator);      
      
      while(fts_iterator_has_more( &iterator))
	{
	  fts_object_t **this_clones = NULL;      
	  fts_object_t **old_clones;
	  fts_atom_t key;
	  int i;
      
      /* get preset clones */
	  fts_iterator_next( &iterator, &key);
	  fts_hashtable_get(&old->hash, &key, &a);
      
	  old_clones = (fts_object_t **)fts_get_pointer(&a);

	  /* copy old clones of matching class from old object */
	  for(i=0; i<n; i++)
	    {
	      if(old_clones[i] != NULL && fts_object_get_class(old_clones[i]) == fts_object_get_class(this->objects[i]))
		{
		  if(this_clones == NULL)
		    this_clones = preset_get_or_add(this, &key);
	      
		  this_clones[i] = old_clones[i];
		  fts_object_refer(this_clones[i]);
		}
	    }
	}

      fts_set_int(&a, old->persistence);
      preset_persistence(o, 0, 0, 1, &a);
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
  fts_preset_t *this = (fts_preset_t *)o;
  int i;

  this->persistence = 0;

  /* check arguments */
  for(i=0; i<ac; i++)
    {
      if(fts_is_object(at + i))
	{
	  fts_object_t *obj = fts_get_object(at + i);
	  
	  if(!preset_check_object(this, obj))
	    {
	      fts_symbol_t name = fts_object_get_class_name(obj);
	      fts_object_set_error(o, "cannot handle object of class %s", name);
	      return;
	    }
	}
      else
	{
	  fts_object_set_error(o, "arguments of object required", i);
	  return;
	}
    }

  /* init hash table */
  fts_hashtable_init( &this->hash, FTS_HASHTABLE_SMALL);

  this->objects = (fts_object_t **)fts_malloc(sizeof(fts_object_t *) * ac);

  for(i=0; i<ac; i++)
    {
      fts_object_t *obj = fts_get_object(at + i);
      this->objects[i] = obj;
      fts_object_refer(obj);
    }

  this->n_objects = ac;
  this->current = 0;
}

static void
preset_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_preset_t *this = (fts_preset_t *)o;
  int i;

  preset_clear(o, 0, 0, 0, 0);

  for(i=0; i<this->n_objects; i++)
    fts_object_release(this->objects[i]);
}

static void
preset_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_preset_t), preset_init, preset_delete);

  fts_class_message_varargs(cl, fts_s_name, fts_name_set_method); 
  fts_class_message_varargs(cl, fts_s_persistence, preset_persistence);
  fts_class_message_varargs(cl, fts_s_update_gui, preset_update_gui); 

  fts_class_message_varargs(cl, fts_s_dump, preset_dump);
  fts_class_message_varargs(cl, fts_s_redefine, preset_redefine);

  fts_class_message_varargs(cl, sym_new_preset, preset_new_preset);
  fts_class_message_varargs(cl, sym_dump_mess, preset_dump_mess);

  fts_class_message_varargs(cl, fts_s_get_array, preset_get_array);

  fts_class_message_varargs(cl, fts_new_symbol("store"), preset_store);
  fts_class_message_varargs(cl, fts_new_symbol("recall"), preset_recall);
  fts_class_message_varargs(cl, fts_s_clear, preset_clear);

  fts_class_inlet_int(cl, 0, preset_recall);
  fts_class_inlet_int(cl, 1, preset_store);
  fts_class_outlet_int(cl, 0);
}

void
fts_kernel_preset_init(void)
{
  sym_new_preset = fts_new_symbol("new_preset");
  sym_dump_mess = fts_new_symbol("dump_mess");

  fts_preset_class = fts_class_install(fts_s_preset, preset_instantiate);
  preset_dumper_type = fts_class_install(NULL, preset_dumper_instantiate);
}

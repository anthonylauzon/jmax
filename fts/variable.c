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


#include <stdlib.h>
#include <string.h>

#include <fts/fts.h>

#include <ftsprivate/object.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/variable.h>

fts_heap_t *definition_heap = NULL;
fts_heap_t *definition_listener_heap = NULL;

/****************************************************************************
 *
 *  definitions (low level interface)
 *
 */

struct fts_definition_listener
{
  fts_object_t *object;
  fts_definition_listener_t *next;
};

static fts_definition_t *
fts_definition_new(fts_symbol_t name)
{
  fts_definition_t *def = (fts_definition_t *)fts_heap_zalloc(definition_heap);
  
  def->name = name;
  fts_set_void(&def->value);
  def->listeners = NULL;

  return def;
}

static void
define_spost_description(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_spost_object_description_args( (fts_bytestream_t *)fts_get_object(at), o->argc-2, o->argv+2);
}

fts_definition_t *
fts_definition_get(fts_patcher_t *scope, fts_symbol_t name)
{
  fts_hashtable_t *hash = fts_patcher_get_definitions(scope);
  fts_definition_t *def = NULL;
  fts_atom_t k, a;

  fts_set_symbol(&k, name);

  if(hash)
    {
      if(fts_hashtable_get(hash, &k, &a))
	return (fts_definition_t *)fts_get_pointer(&a);
    }
  else
    {
      hash = (fts_hashtable_t *)fts_malloc(sizeof(fts_hashtable_t));
      
      fts_hashtable_init(hash, fts_symbol_class, FTS_HASHTABLE_MEDIUM);
      fts_patcher_set_definitions(scope, hash);
    }

  def = fts_definition_new(name);
  
  fts_set_pointer(&a, def);
  fts_hashtable_put(hash, &k, &a);

  return def;
}

void 
fts_definition_add_listener(fts_definition_t *def, fts_object_t *obj)
{
  fts_definition_listener_t *l;

  /* make sure that we don't add the same listener twice */
  for(l = def->listeners; l != NULL; l = l->next)
    {
      if (l->object == obj)
	return;
    }

  /* create new list entry */
  l = (fts_definition_listener_t *)fts_heap_alloc(definition_listener_heap);

  /* add it to the beginning of the list of the definition */
  l->object = obj;
  l->next = def->listeners;
  def->listeners = l;
}

void 
fts_definition_remove_listener(fts_definition_t *def, fts_object_t *obj)
{
  fts_definition_listener_t *l;

  /* search for listener at the definition */
  for(l = def->listeners; l != NULL; l = l->next)
    {
      /* just set it to NULL */
      if (l->object == obj)
	{
	  l->object = NULL;
	  return;
	}
    }
}

static void 
definition_recompute_listeners(fts_definition_t *def)
{
  fts_definition_listener_t **p = &def->listeners;

  while(*p != NULL)
    {
      fts_object_t *obj = (*p)->object;

      if (obj != NULL)
	{
	  fts_object_recompute(obj);
	  p = &(*p)->next;
	}
      else
	{
	  fts_definition_listener_t *remove = *p;

	  /* remove empty listener entry */
	  *p = remove->next;
	  fts_heap_free(remove, definition_listener_heap);
	}
    }
}

void
fts_definition_update(fts_definition_t *def, const fts_atom_t *value)
{
  fts_definition_set_value(def, value);
  definition_recompute_listeners(def);  
}

/*************************************************************
 *
 *  names (high level API)
 *
 *
 */
void
fts_name_set_value(fts_patcher_t *patcher, fts_symbol_t name, const fts_atom_t *value)
{
  fts_patcher_t *scope = fts_patcher_get_scope(patcher);
  fts_definition_t *def = fts_definition_get(scope, name);

  if(!fts_atom_equals(value, &def->value))
    fts_definition_update(def, value);
}

void
fts_name_add_listener(fts_patcher_t *patcher, fts_symbol_t name, fts_object_t *obj)
{
  fts_patcher_t *scope = fts_patcher_get_scope(patcher);
  fts_definition_t *def = fts_definition_get(scope, name);
  fts_atom_t a;

  fts_definition_add_listener(def, obj);

  fts_set_pointer(&a, def);
  obj->name_refs = fts_list_prepend(obj->name_refs, &a);
}

void
fts_name_remove_listener(fts_patcher_t *patcher, fts_symbol_t name, fts_object_t *obj)
{
  fts_patcher_t *scope = fts_patcher_get_scope(patcher);
  fts_definition_t *def = fts_definition_get(scope, name);

  fts_definition_remove_listener(def, obj);
}

fts_atom_t *
fts_name_get_value(fts_patcher_t *patcher, fts_symbol_t name)
{
  fts_patcher_t *scope = fts_patcher_get_scope(patcher);
  fts_definition_t *def = fts_definition_get(scope, name);

  return &def->value;
}

fts_symbol_t
fts_name_get_unused(fts_patcher_t *patcher, fts_symbol_t name)
{
  fts_patcher_t *scope = fts_patcher_get_scope(patcher);
  fts_hashtable_t *hash = fts_patcher_get_definitions(scope);
  fts_atom_t a, k;

  fts_set_symbol(&k, name);

  if(hash != NULL && fts_hashtable_get(hash, &k, &a))
    {
      fts_definition_t *def = fts_get_pointer(&a);
	      
      if(!fts_is_void(&def->value))
	{
	  const char *str = name;
	  int len = strlen(str);
	  char *new_str = alloca((len + 10) * sizeof(char));
	  int num = 0;
	  int dec = 1;
	  int i;
	  
	  /* separate base name and index */
	  for(i=len-1; i>=0; i--) 
	    {
	      if(len == (i + 1) && str[i] >= '0' && str[i] <= '9')
		num += (str[len = i] - '0') * dec;
	      else
		new_str[i] = str[i];
	      
	      dec *= 10;
	    }
	  
	  /* generate new name */
	  sprintf(new_str + len, "%d", ++num);
	  name = fts_new_symbol(new_str);
	  fts_set_symbol(&k, name);
	  
	  while(fts_hashtable_get(hash, &k, &a)) 
	    {
	      fts_definition_t *def = fts_get_pointer(&a);
	      
	      if(fts_is_void(&def->value))
		break;
	      
	      sprintf(new_str + len, "%d", ++num);
	      name = fts_new_symbol(new_str);
	      fts_set_symbol(&k, name);
	    }
	}
    }

  return name;
}

void
fts_name_method( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(ac > 0 && fts_is_symbol(at) && fts_object_get_patcher(o) != NULL)
    {
      fts_object_set_name(o, fts_get_symbol(at));
      /*fts_patcher_set_dirty(fts_object_get_patcher(o), 1);*/
    }
}

/****************************************************************************
 *
 * "define" object
 *
 */

typedef struct
{
  fts_object_t o;
  fts_symbol_t name;
  fts_atom_t value;
} define_t;

static void 
define_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *) o;  

  if(ac > 1 && fts_is_symbol(at))
    {
      fts_patcher_t *patcher = fts_object_get_patcher(o);
      fts_symbol_t name = fts_name_get_unused(patcher, fts_get_symbol(at));

      fts_name_set_value(patcher, name, (fts_atom_t *)(at + 1));
      this->name = name;
      this->value = at[1];

      fts_atom_refer(&this->value);
    }
  else
    fts_object_set_error(o, "bad arguments");
}

static void 
define_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *) o;  

  fts_name_set_value(fts_object_get_patcher( o), this->name, fts_null);
  fts_atom_release(&this->value);
}

static void
define_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(define_t), define_init, define_delete);
  
  fts_class_message_varargs(cl, fts_s_spost_description, define_spost_description); 
}

typedef struct
{
  fts_object_t o;
  fts_atom_t *arg;
  fts_atom_t def;
} args_t;

static void
args_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  args_t *this = (args_t *)o;
  int n = ac / 2;

  if((ac == 1 || ac == 2) && fts_is_int(at))
    {
      fts_tuple_t *args = fts_patcher_get_args(fts_object_get_patcher(o));
      fts_atom_t *ptr = fts_tuple_get_atoms(args);
      int size = fts_tuple_get_size(args);
      int index;
      
      fts_set_void(&this->def);
      
      index = fts_get_int(at);
      
      if(args && index < size)
	this->arg = ptr + index;
      else if(ac > 1)
	{
	  /*fts_atom_assign(&this->def, at + 1);*/
	  
	  this->arg = &this->def;
	}
      else
	{
	  fts_object_set_error(o, "argument %d is not defined for this patcher", index);
	  return;
	}

      fts_name_add_listener(fts_object_get_patcher(o), fts_s_args, o);
    }
  else
    fts_object_set_error(o, "bad arguments");
}

static void
args_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  args_t *this = (args_t *)o;

  fts_set_void(&this->def);
}

static void
args_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(args_t), args_init, args_delete);
  
  fts_class_outlet_varargs(cl, 0);
}

void 
fts_kernel_variable_init(void)
{
  fts_class_install( fts_s_define, define_instantiate);
  /*fts_class_install( fts_s_args, args_instantiate);*/

  definition_heap = fts_heap_new(sizeof(fts_definition_t));
  definition_listener_heap = fts_heap_new(sizeof(fts_definition_listener_t));
}

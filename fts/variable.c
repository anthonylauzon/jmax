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

#include <ftsprivate/object.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/variable.h>

fts_heap_t *definition_heap = NULL;

/****************************************************************************
 *
 *  names and definitions
 *
 */


static fts_definition_t *
fts_definition_new(void)
{
  fts_definition_t *def = NULL;

  if(definition_heap == NULL)
    definition_heap = fts_heap_new(sizeof(fts_definition_t));

  def = (fts_definition_t *)fts_heap_zalloc(definition_heap);
  
  fts_set_void(&def->value);
  fts_objectlist_init(&def->listeners);

  return def;
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
      
      fts_hashtable_init(hash, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_MEDIUM);
      fts_patcher_set_definitions(scope, hash);
    }

  def = fts_definition_new();
  
  fts_set_pointer(&a, def);
  fts_hashtable_put(hash, &k, &a);

  return def;
}

void 
fts_definition_add_listener(fts_definition_t *def, fts_object_t *obj)
{
  fts_objectlist_insert(&def->listeners, obj);
}

void 
fts_definition_remove_listener(fts_definition_t *def, fts_object_t *obj)
{
  fts_objectlist_remove(&def->listeners, obj);
}

void
fts_name_define(fts_patcher_t *patcher, fts_symbol_t name, fts_atom_t *value)
{
  fts_patcher_t *scope = fts_patcher_get_top_level(patcher);
  fts_definition_t *def = fts_definition_get(scope, name);

  if(!fts_atom_equals(value, &def->value))
    {
      fts_objectlist_cell_t *p;

      fts_atom_assign(&def->value, value);      
      
      for(p = fts_objectlist_get_head( &def->listeners); p; p = fts_objectlist_get_next(p))
	fts_object_recompute(fts_objectlist_get_object(p));
    }
}

void
fts_name_undefine(fts_patcher_t *patcher, fts_symbol_t name)
{
  fts_patcher_t *scope = fts_patcher_get_top_level(patcher);
  fts_definition_t *def = fts_definition_get(scope, name);

  if(!fts_is_void(&def->value))
    {
      fts_objectlist_cell_t *p;

      fts_atom_void(&def->value);
      
      for(p = fts_objectlist_get_head( &def->listeners); p; p = fts_objectlist_get_next(p))
	fts_object_recompute(fts_objectlist_get_object(p));
    }
}

void
fts_name_add_listener(fts_patcher_t *patcher, fts_symbol_t name, fts_object_t *obj)
{
  fts_patcher_t *scope = fts_patcher_get_top_level(patcher);
  fts_definition_t *def = fts_definition_get(scope, name);
  fts_atom_t a;

  fts_definition_add_listener(def, obj);

  fts_set_pointer(&a, def);
  obj->name_refs = fts_list_prepend(obj->name_refs, &a);
}

void
fts_name_remove_listener(fts_patcher_t *patcher, fts_symbol_t name, fts_object_t *obj)
{
  fts_patcher_t *scope = fts_patcher_get_top_level(patcher);
  fts_definition_t *def = fts_definition_get(scope, name);

  fts_definition_remove_listener(def, obj);
}

fts_atom_t *
fts_name_get_value(fts_patcher_t *patcher, fts_symbol_t name)
{
  fts_patcher_t *scope = fts_patcher_get_top_level(patcher);
  fts_definition_t *def = fts_definition_get(scope, name);

  return &def->value;
}

fts_symbol_t
fts_name_get_unused(fts_patcher_t *patcher, fts_symbol_t name)
{
  fts_patcher_t *scope = fts_patcher_get_top_level(patcher);
  fts_hashtable_t *hash = fts_patcher_get_definitions(scope);

  return fts_hashtable_get_unused_symbol(hash, name);
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
} define_t;

static void 
define_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *) o;  

  if(ac > 1 && fts_is_symbol(at))
    {
      fts_patcher_t *patcher = fts_object_get_patcher(o);
      fts_symbol_t name = fts_name_get_unused(patcher, fts_get_symbol(at));

      fts_name_define(patcher, name, (fts_atom_t *)(at + 1));
      this->name = name;
    }
  else
    fts_object_set_error(o, "bad arguments");
}

static void 
define_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *) o;  

  fts_name_undefine(fts_object_get_patcher( o), this->name);
}

static void
define_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(define_t), define_init, define_delete);
}

void 
fts_kernel_variable_init(void)
{
  fts_class_install( fts_new_symbol( "define"), define_instantiate);
}

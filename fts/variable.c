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
#include <ftsconfig.h>

#include <stdlib.h>
#include <string.h>
#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include <fts/fts.h>

#include <ftsprivate/object.h>
#include <ftsprivate/patcherobject.h>
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
  def->global = NULL;

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

    fts_hashtable_init( hash, FTS_HASHTABLE_MEDIUM);
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
  for(l=def->listeners; l!=NULL; l=l->next)
  {
    if(l->object == obj)
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

/* fts_patcher_definition_recompute_listeners(fts_definition_t *def) */
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

  if(def->global != NULL)
  {
    fts_definition_set_value(def->global, value);
    definition_recompute_listeners(def->global);
    
    /* undefined names are always local */
    if(fts_is_void(value))
      def->global = NULL;
  }
}

void
fts_definition_set_global(fts_definition_t *def)
{
  if(!fts_is_void(&def->value) && def->global == NULL)
  {
    fts_definition_t *global = fts_definition_get(fts_get_root_patcher(), def->name);
    fts_atom_t *global_value = fts_definition_get_value(global);
    
    if(fts_is_void(global_value))
    {
      /* set global definition to local value */
      fts_definition_update(global, fts_definition_get_value(def));
      
      /* set global (cash global definiton) */
      def->global = global;
    }
  }
}

void
fts_definition_set_local(fts_definition_t *def)
{
  fts_definition_t *global = def->global;

  if(def->global != NULL)
  {
    /* reset value of global definition */
    fts_definition_update(global, fts_null);
    
    /* set local */
    def->global = NULL;
  }
}

/*************************************************************
 *
 *  names (high level API)
 *
 */
void
fts_name_set_value(fts_patcher_t *patcher, fts_symbol_t name, const fts_atom_t *value)
{
  fts_patcher_t *scope = fts_patcher_get_scope(patcher);
  fts_definition_t *def = fts_definition_get(scope, name);

  if(!fts_atom_identical(value, &def->value))
    fts_definition_update(def, value);
}

void
fts_name_add_listener(fts_patcher_t *patcher, fts_symbol_t name, fts_object_t *obj)
{
  fts_patcher_t *scope = fts_patcher_get_scope(patcher);
  fts_definition_t *def = fts_definition_get(scope, name);

  fts_definition_add_listener(def, obj);
  
  if(fts_is_void(&def->value))
  {
    def = fts_definition_get(fts_get_root_patcher(), name);

    fts_definition_add_listener(def, obj);
  }
}

void
fts_name_remove_listener(fts_patcher_t *patcher, fts_symbol_t name, fts_object_t *obj)
{
  fts_patcher_t *scope = fts_patcher_get_scope(patcher);
  fts_definition_t *def = fts_definition_get(scope, name);

  fts_definition_remove_listener(def, obj);

  if(fts_is_void(&def->value))
  {
    def = fts_definition_get(fts_get_root_patcher(), name);
    
    fts_definition_remove_listener(def, obj);
  }
}

fts_atom_t *
fts_name_get_value(fts_patcher_t *patcher, fts_symbol_t name)
{
  fts_patcher_t *scope = fts_patcher_get_scope(patcher);
  fts_definition_t *def = fts_definition_get(scope, name);
  
  /* if there is no local value get the global */
  if(fts_is_void(&def->value))
    def = fts_definition_get(fts_get_root_patcher(), name);

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
      const char *str = fts_symbol_name(name);
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
fts_kernel_variable_init(void)
{
  definition_heap = fts_heap_new(sizeof(fts_definition_t));
  definition_listener_heap = fts_heap_new(sizeof(fts_definition_listener_t));
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
